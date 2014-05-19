@echo off

 netsh interface ipv6 show interface

set /p MSLOOPIF="Please select a interface: "

netsh interface ipv6 add neighbor interface=%MSLOOPIF% aaaa::bb 00-00-00-00-00-bb
 
netsh interface ipv6 add neighbor interface=%MSLOOPIF% aaaa::cc 00-00-00-00-00-cc
rem www.smeshlink.com