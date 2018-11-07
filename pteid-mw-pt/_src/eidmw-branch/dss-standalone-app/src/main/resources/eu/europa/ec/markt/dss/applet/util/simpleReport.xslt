<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns="http://www.w3.org/1999/xhtml"
                xmlns:dss="http://dss.markt.ec.europa.eu/validation/diagnostic">

    <xsl:output method="xml"
                doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"
                doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN" indent="yes"/>
                
	
	

    <xsl:template match="/dss:SimpleReport">
      
      
        <html>
            <head>
                <title>Validation Simple Report</title>
                <style type="text/css">
                    body {
                        font-family: sans-serif;
                    }

                    th, td {
                    	padding-left: 0px;
                        text-align: left;
                        vertical-align: top;
                    }

                    th {
                        font-weight: inherit;
                        width: 30%;
                    }

                    td {
                        width: 70%;
                    }

                    tr.validationPolicy .validationPolicy-description {
                        font-size: 70%;
                        border-top: 1px solid gray;
                    }

                    tr.signature-start, tr.signature-start th, tr.signature-start{
                        border-top: 1px solid gray;
                        font-size:120%;
                        font-weight: bold;
                    }
                    
                    tr.signature-start td {
                    	border-top: 1px solid gray;
                    	font-weight: bold;
                    }

                    th.indication {
                        font-weight: bold;
                        font-size:150%;
                    }

                    th.indication .indication-icon {
                        font-size:150%;
                        margin-right: 0.5em;
                        font-style: italic;
                    }

                    .VALID {
                        color: green;
                    }

                    .INDETERMINATE {
                        color: orange;
                    }

                    .INVALID {
                        color: red;
                    }
					
					tr.documentName th {
						font-weight: bold;
						padding-left: 0px;
					}
					
                    td.signatureLevel {
                        font-weight: bold;
                    }

                    tr.documentInformation {
                        color: darkgreen;
                    }

                    tr.documentInformation th {
                        padding-left: 2em;
                    }

                    tr.documentInformation-header, tr.documentInformation-header th, tr.documentInformation-header td {
                        border-top: 1px solid gray;
                    }
                    
                    tr.signedBy td, tr.signedBy th {
                        border-top: 1px solid gray;
                    }
                    
                    tr.paramHeader, tr.paramHeader th {
                    	border-top: 1px solid gray;
                    	font-weight: bold;
                    }
                    
                    tr.sigLast th, tr.sigLast td {
                    	padding-bottom: 15px
                    }

                    tr.documentInformation-header th {
                        padding-left: 0;
                        font-weight: bold;
                    }
                </style>
            </head>
            <body>
                <table>
                    <xsl:apply-templates/>             	
    	
       		 <tr class="validationPolicy">
            	<th>STR_POL0</th>
            	<td>
                <div class="validationPolicy-name">
                    STR_POL1
                </div>
                <div class="validationPolicy-description">
                    STR_POL2
                </div>
            </td>
        		</tr>
                </table>
            </body>


        </html>
    </xsl:template>
    
    <xsl:template match="dss:DocumentName">
    	<tr class="documentName">
            <th>STR_DOCNAME</th>
            <td>
                <xsl:value-of select="./text()"/>
            </td>
        </tr>
    </xsl:template>
	<xsl:template match="dss:ValidationTime"/>
	
    <xsl:template match="dss:Signature">
        <xsl:variable name="indicationClass" select="dss:Indication/text()"/>
        <xsl:variable name="info_msg" select="dss:Info/text()"/>
	<tr class="signature-start">
		<th>
			<xsl:value-of select="dss:Indication/text()" />
		</th>

		<td>
            <xsl:for-each select="dss:Info">
			<xsl:if test="contains(text(), 'Failed to get network resource')">STR_NETWORK_ERROR</xsl:if>
            </xsl:for-each>
		</td>

	</tr>
        <!-- 
        <xsl:apply-templates select="dss:SubIndication">
        </xsl:apply-templates>
        <xsl:apply-templates select="dss:Info">
        </xsl:apply-templates>
         -->
        
         <tr class="paramHeader">
            <th colspan="2">
            STRING
            </th>
        </tr>
       
       <xsl:variable name="indication" select="dss:SubIndication/text()"/>
         <tr class="paramTrusted">
            <th>STRING</th>
            <td>
                 
            </td>
        </tr>

        <tr class="paramTS">
            <th>STRING</th>
            <td>
                <xsl:value-of select="dss:TimestampPresent"/>
            </td>
        </tr>
         <tr class="paramLTV">
            <th>STRING</th>
            <td>
                <xsl:value-of select="dss:LTV"/>
            </td>
        </tr>
        <tr class="signedBy">
            <th>STRING</th>
            <td>
                <xsl:value-of select="dss:SignedBy"/>
            </td>
        </tr>
        <tr class="sigLast">
            <th>STRING</th>
            <td>
                <div>
                    <xsl:value-of select="dss:SigningTime"/>
                </div>
            </td>
        </tr>
            
    </xsl:template>
   
    <xsl:template match="dss:Policy"/>
    

</xsl:stylesheet>
