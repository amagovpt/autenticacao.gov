#!/bin/bash
echo \<?xml version=\"1.0\" encoding=\"utf-8\"?\> 						>  beid.jnlp
echo \<jnlp codebase=\"file:///`pwd`\" href=\"beid.jnlp\"\> 					>> beid.jnlp
echo \<information\>						 				>> beid.jnlp
echo \<title\>Java Binding to Belgium eID Middleware 3.5\</title\>				>> beid.jnlp
echo \<vendor\>Fedict\</vendor\>								>> beid.jnlp
echo \<offline-allowed/\>									>> beid.jnlp
echo \</information\>										>> beid.jnlp
echo \<security\>										>> beid.jnlp
echo \<all-permissions/\>									>> beid.jnlp
echo \</security\>										>> beid.jnlp
echo \<resources os=\"Windows\"\>\<nativelib href = \"beid35JavaWrapper-win.jar\" /\>\</resources\>	>> beid.jnlp
echo \<resources os=\"Linux\"\>\<nativelib href = \"beid35JavaWrapper-linux.jar\" /\>\</resources\>	>> beid.jnlp
echo \<resources os=\"Mac OS X\"\>\<nativelib href = \"beid35JavaWrapper-mac.jar\" /\>\</resources\> 	>> beid.jnlp
echo \<component-desc /\>									>> beid.jnlp
echo \</jnlp\>											>> beid.jnlp

