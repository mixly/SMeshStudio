/*
 *
 * mbed Simple SMTP Client
 * Copyright (c) 2012 Tadao Iida
 * Released under the MIT License: http://mbed.org/license/mit
 */ 

//Debug is disabled by default
#if 0
//Enable debug
#define __DEBUG__
#include <cstdio>
#define DBG(x, ...) std::printf("[DBG]"x"\r\n", ##__VA_ARGS__); 

#else
//Disable debug
#define DBG(x, ...) 

#endif

/** @file
 * @brief Simple SMTP Client
 */
 
#include "mbed.h"
#include "EthernetInterface.h"
#include "SimpleSMTPClient.h"

SimpleSMTPClient::SimpleSMTPClient() : smtp()
{


}


// Copyright (c) 2010 Donatien Garnier (donatiengar [at] gmail [dot] com)
int SimpleSMTPClient::base64enc(const char *input, unsigned int length, char *output, int outputlen) {
  static const char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  unsigned int c, c1, c2, c3;
 
  if (outputlen < (((length-1)/3)+1)<<2) return -1;
 
  for(unsigned int i = 0, j = 0; i<length; i+=3,j+=4) {
    c1 = ((((unsigned char)*((unsigned char *)&input[i]))));
    c2 = (length>i+1)?((((unsigned char)*((unsigned char *)&input[i+1])))):0;
    c3 = (length>i+2)?((((unsigned char)*((unsigned char *)&input[i+2])))):0;
 
    c = ((c1 & 0xFC) >> 2);
    output[j+0] = base64[c];
    c = ((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4);
    output[j+1] = base64[c];
    c = ((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6);
    output[j+2] = (length>i+1)?base64[c]:'=';
    c = (c3 & 0x3F);
    output[j+3] = (length>i+2)?base64[c]:'=';
  }
  output[(((length-1)/3)+1)<<2] = '\0';
  return 0;
}
 
// SMTP ReceiveMessage 
int SimpleSMTPClient::receiveMessage(int code) {
    int i;
    char buf[1500];
 
    do{
        i = smtp.receive(buf, sizeof(buf));
        buf[i]='\0';
        printf("Recv>>%s[%d] \r\n",buf,i);    
    }while(atoi(buf) != code && i > 0 );
    
    // Check return code
    if (i < 0 )
        return -1; 
    
    return 0;
}
 
int SimpleSMTPClient::sendmail (char *host, char *user, char *pwd, char *domain,char *port,SMTPAuth auth) {

    int ret = -1;
    char ehlo[128];
 
    smtp.set_blocking(false, 1500); 
    smtp.connect(host, atoi(port)) ;
    if ( !smtp.is_connected() ){
        printf("Connection Error!\r\n");
        smtp.close();
        return -1; 
    }
    
    // send request
    wait_ms(100);
    sprintf(ehlo,"EHLO %s\r\n",domain);
    #ifdef __DEBUG__
        DBG("SEND %s(%d)", ehlo,strlen(ehlo));
    #endif    
    smtp.send(ehlo, strlen(ehlo));
    if (receiveMessage(250)){
        smtp.close();
        return -1;
    } 

    if ( auth == SMTP_AUTH_PLAIN ){
        if ( user && pwd ) {
            // SMTP auth PLAIN
            char tmp[80], buf[100];
            int len;
            snprintf(tmp, sizeof(tmp), "%s%c%s%c%s",user, 0, user, 0, pwd);
            len = strlen(user)*2 + strlen(pwd) + 2;
            base64enc(tmp, len, buf, sizeof(buf));
            smtp.send("AUTH PLAIN ", 11);
            smtp.send(buf, strlen(buf));
            smtp.send("\r\n", 2);
            #ifdef __DEBUG__
              DBG("base64[%s]\r\n", buf);
            #endif            
            if (receiveMessage(235)){
                smtp.send("QUIT\r\n", 6);
                smtp.close();
            }
        }
    }else if ( auth == SMTP_AUTH_LOGIN) {
        if ( user && pwd ) {
            // SMTP auth
            char tmp[80], buf[100];
            int len;

            smtp.send("AUTH LOGIN ", 11);
            smtp.send("\r\n", 2);

            snprintf(tmp, sizeof(tmp), "%s",user);
            len = strlen(user) ;
            base64enc(tmp, len, buf, sizeof(buf));
            smtp.send(buf, strlen(buf));
            smtp.send("\r\n", 2);
  
            snprintf(tmp, sizeof(tmp), "%s",pwd);
            len = strlen(pwd) ;
            base64enc(tmp, len, buf, sizeof(buf));
            smtp.send(buf, strlen(buf));
            smtp.send("\r\n", 2);

            #ifdef __DEBUG__
              DBG("base64[%s]\r\n", buf);
            #endif            
            if (receiveMessage(235)){
                smtp.send("QUIT\r\n", 6);
                smtp.close();
            }     
        }   
    }        

    smtp.send("MAIL FROM:<", 11);
    smtp.send(getFromAddress(), strlen(getFromAddress()));
    smtp.send(">\r\n", 3);
    if (receiveMessage(250)) {
        smtp.close();
        return -1;
    }
           

    char tmp[128],addr[128];
    int i = 0;
    strcpy(tmp,getToAddress());
    
    while( tmp[i] != '\0' ){
        int j = 0;
        while( tmp[i] != ',' ){
            addr[j] = tmp[i];

            if( tmp[i] == '\0' ) break;
            j++;
            i++;
        }
        addr[j]='\0';
        
        #ifdef __DEBUG__
          DBG("ToAddress[%s]\r\n", addr);
        #endif
        
        smtp.send("RCPT TO: <", 10);
        smtp.send(addr, strlen(addr));
        smtp.send(">\r\n", 3);
        if(tmp[i]==',') i++;
        
        if (receiveMessage(250)) goto exit;
    }
    ret = makeHeader();
    if ( ret == -1 ) {
        printf("The headers are less than 256 characters.\r\n");
        goto exit ;
    }
 
    // mail Body
    smtp.send("DATA\r\n", 6);
    if (receiveMessage(354)) goto exit; 
    
    smtp.send(getHeader(), strlen(getHeader()));
    smtp.send(getMessage(), strlen(getMessage()));
    smtp.send("\r\n.\r\n", 5);
    if (receiveMessage(250)) goto exit;
    #ifdef __DEBUG__
        DBG("Header %s\r\n", getHeader());
        DBG("Message %s\r\n", getMessage());
    #endif
    
    ret = 0;

    smtp.send("QUIT\r\n", 6);
    if (receiveMessage(221)) goto exit;
 
exit:
    smtp.close();

    return ret;
}

int SimpleSMTPClient::setMessage(char *sub,char *msg)
{
    int n;
    n = strlen(sub);
    if( n >= 64 )
        return -1;
    n = strlen(msg);
    if( n >= 1244 )
        return -1;    
    strcpy(subject,sub);        
    strcpy(message,msg);
    return 0;
}

int SimpleSMTPClient::addMessage(char *buf)
{
    int n;
    n = strlen(message);
    n = n+strlen(buf);
    if( n >= 1244 )
        return -1;           
    strcat(message,buf);
    return 0;
}

int SimpleSMTPClient::setFromAddress(char *fromaddr)
{
    int n;
    n = strlen(fromaddr);
    if( n <= 0 )
        return -1;
    strcpy(from,fromaddr);

    return n;
}
char* SimpleSMTPClient::getFromAddress()
{
    return from;
}
int SimpleSMTPClient::setToAddress(char *toaddr)
{
    int n;
    n = strlen(toaddr);
    if( n <= 0 && 128 <= n)
        return -1;    
    strcpy(to,toaddr);

    return n;
}
char* SimpleSMTPClient::getToAddress()
{
    return to;
}

char* SimpleSMTPClient::getSubject()
{
    return subject;
}

char* SimpleSMTPClient::getHeader()
{
    return header;
}

char* SimpleSMTPClient::getMessage()
{
    return message;
}

int SimpleSMTPClient::makeHeader(void)
{
    strcpy(header, "From: ");
    strcat(header, getFromAddress());
    strcat(header, "\r\n");
    strcat(header, "To: " );
    strcat(header, getToAddress());
    strcat(header,  "\r\n");
    strcat(header, "Subject: ");
    strcat(header, getSubject());
    strcat(header, "\r\n\r\n") ;
    
    if ( strlen(header) > 256 )
        return -1;
    return 0;
}

void SimpleSMTPClient::clearMessage(void)
{
    memset(message, 0, sizeof(message)); 
}

int SimpleSMTPClient::msgLength(void)
{
    return strlen(message);
}