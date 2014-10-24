/*
 * 
 * mbed Simple SMTP Client
 * Copyright (c) 2012 Tadao Iida
 * Released under the MIT License: http://mbed.org/license/mit
 */ 

/** @file
 * @brief Simple SMTP Client
 */

#ifndef SimpleSMTPC_h
#define SimpleSMTPC_h

#include "EthernetInterface.h"

///SMTP authentication
enum SMTPAuth {
    SMTP_AUTH_NONE, ///<No authentication
    SMTP_AUTH_PLAIN, ///<AUTH PLAIN authentication
    SMTP_AUTH_LOGIN
};

///SimpleSMTP client results
enum SMTPResult
{
    SMTP_AUTH_OK = 235, ///<Authentication successful
    SMTP_OK = 250, ///<Requested mail action okay, completed
    SMTP_INPUT = 354 ///<Start mail input
};
#define DEBUG

#define SMTP_TIMEOUT 15000 // ms

class SimpleSMTPClient
{
public:
  /**
  Instantiate the SimpleSMTP client
  */
  SimpleSMTPClient();

    /** send mail
     * @param host mail server
     * @param data mail body
     * @param user auth username
     * @param pwd auth password 
     * @param domain
     * @param port mail port
     * @param auth SMTP auth
     * @return 0:success, -1:failue
     */
    int sendmail (char *host, char *user, char *pwd,char *domain,char *port,SMTPAuth auth);
    
    /** setMessage
    * @param sub Subject <The Subject are less than 64 characters.>
    * @param msg Message
    * @return 0:success, -1:failue
    */
    int setMessage(char *sub,char *msg);
    
    /** addMessage
    * @param msg Message 
    * @return 0:success, -1:failue
    */    
    int addMessage(char *msg);
         
    /** setFromAddress
    * @param from mail address
    * @return character count, -1:failue
    */     
    int setFromAddress(char *from);
    
    /** setToAddress <The ToAddress are less than 128 characters.>
    * @param to mail address
    * @return character count, -1:failue
    */
    int setToAddress(char *to);
    
    /** clearMessage 
    *    
    */
    void clearMessage(void);

    /** msgLength
    *
    */
    int msgLength(void);    

 
private:
    int base64enc(const char *input, unsigned int length, char *output, int outputlen); 
    int receiveMessage(int code);
    char* getFromAddress(void);
    char* getToAddress(void);   
    char* getSubject();
    char* getHeader();
    char* getMessage();
    int makeHeader(void);
    
    TCPSocketConnection smtp;
    char header[256];
    char body[1500];    
    char from[64];
    char to[128];
    char subject[64];
    char message[1244];
};

#endif
