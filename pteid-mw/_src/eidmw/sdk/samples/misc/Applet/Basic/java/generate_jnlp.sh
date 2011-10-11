#!/bin/bash
echo \<?xml version=\"1.0\" encoding=\"utf-8\"?\> 						>  pteid.jnlp
echo \<jnlp codebase=\"file:///`pwd`\" href=\"pteid.jnlp\"\> 					>> pteid.jnlp
echo \<information\>						 				>> pteid.jnlp
echo \<title\>Java Binding to Portugal eID Middleware 3.5\</title\>				>> pteid.jnlp
echo \<vendor\>Fedict\</vendor\>								>> pteid.jnlp
echo \<offline-allowed/\>									>> pteid.jnlp
echo \</information\>										>> pteid.jnlp
echo \<security\>										>> pteid.jnlp
echo \<all-permissions/\>									>> pteid.jnlp
echo \</security\>										>> pteid.jnlp
echo \<resources os=\"Windows\"\>\<nativelib href = \"pteid35JavaWrapper-win.jar\" /\>\</resources\>	>> pteid.jnlp
echo \<resources os=\"Linux\"\>\<nativelib href = \"pteid35JavaWrapper-linux.jar\" /\>\</resources\>	>> pteid.jnlp
echo \<resources os=\"Mac OS X\"\>\<nativelib href = \"pteid35JavaWrapper-mac.jar\" /\>\</resources\> 	>> pteid.jnlp
echo \<component-desc /\>									>> pteid.jnlp
echo \</jnlp\>											>> pteid.jnlp

