<?xml version="1.0" encoding="euc-kr" ?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<!-- Global Definitions -->
	<xsl:output method="html" omit-xml-declaration="no" encoding="euc-kr" indent="no"/>
	<xsl:strip-space elements="*" />

	<!-- Main Template -->

	<xsl:template match="/">
		<html>
			<head>
				<title>SoulHunt Log</title>		
				<link type="text/css" rel="stylesheet" href="http://maiet.net/~zelinde/mlog.css"/>
			</head>
			<body leftmargin="10" topmargin="10">
				<table cellpadding="5" cellspacing="5" border="0" width="100%" height="100%">
					<tr height="20">
						<td width="100%" height="20" valign="top" style="background: #EEEEEE;"><H1><B>SoulHunt Log 
						( <xsl:value-of select="log/@date" /> , <xsl:value-of select="log/@time" /> )</B></H1></td>
					</tr>

					<xsl:apply-templates select="/log/header" /> 
					
					<tr height="100%">
						<td valign="top">
							<table cellpadding="1" cellspacing="1" border="0" width="100%">
								<tr>
									<td style="margin-top:4px; margin-bottom: 8px; background: #DBE0E5;" colspan="2">
										<H2>Log</H2>
									</td>
								</tr>
							
								<xsl:apply-templates select="/log/event" /> 
							</table>
						</td>
						
					</tr>
				</table>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="header">
		<tr>
			<td>
				<table cellpadding="2" cellspacing="1" border="0" width="800" border-color="#000000">
				<tr>
					<td colspan="2" style="margin-top:4px; margin-bottom: 8px; background: #DBE0E5;">
						<H2>Header Information</H2>
					</td>
				</tr>
				<xsl:apply-templates select="/log/header/sysinfo" />
				</table>
			</td>
		</tr>
	</xsl:template>

	<xsl:template match="sysinfo">
		<tr>
			<td width="120" class="header_name"> <xsl:value-of select="@name" /> </td>
			<td class="header_value"> <xsl:value-of select="." /> </td>
		</tr>
	</xsl:template>
	
	<xsl:template match="event[@type='information']">
		<tr>
			<td width="60" align="center"><xsl:value-of select="@time"/></td>
			<td style="color: #000000;"><xsl:value-of select="."/></td>
		</tr>
	</xsl:template>
	<xsl:template match="event[@type='warning']">
		<tr>
			<td width="60" align="center"><xsl:value-of select="@time"/></td>
			<td style="color: #DE8200;"><xsl:value-of select="."/></td>
		</tr>
	</xsl:template>
	<xsl:template match="event[@type='error']">
		<tr>
			<td width="60" align="center"><xsl:value-of select="@time"/></td>
			<td style="color: #FF0000;"><xsl:value-of select="."/></td>
		</tr>
	</xsl:template>
	
	
</xsl:stylesheet>

