<?xml version="1.0" encoding="euc-kr" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/TR/xhtml1/strict">
  <xsl:output method="html"/>
  
  <xsl:template match="/">
    <xsl:apply-templates select="//pmd-cpd/duplication" />
  </xsl:template>
  
  <xsl:template match="duplication">
    <table class="section-table" cellpadding="2" cellspacing="0" border="0" width="98%">
      <colgroup>
        <col width="300px"/>
        <col width="50px" />
      </colgroup>

      <tr>
        <td class="sectionheader" colspan="2">Duplication Lines : <xsl:value-of select="@lines" /> , Tokens : <xsl:value-of select="@tokens" />
		</td>
      </tr>

      <tr>
        <td class="sectionheader" width="200">File</td>
		<td class="sectionheader">Line</td>
      </tr>
	  <xsl:apply-templates select="file" />
	  <xsl:apply-templates select="codefragment" />
    </table>
    
    <br/>
    
  </xsl:template>

		<xsl:template match="duplication/file">
			<tr>
				<td><xsl:value-of select="@path" /></td>
				<td><xsl:value-of select="@line" /></td>
			</tr>
			<tr>
			<td colspan="2">
			
			</td>
			</tr>
				
		</xsl:template>	  
		<xsl:template match="duplication/codefragment">
			<tr>
				<td><pre><xsl:value-of select="." /></pre></td>
			</tr>
			<tr>
			<td colspan="2">
			
			</td>
			</tr>
				
		</xsl:template>	  

</xsl:stylesheet>
