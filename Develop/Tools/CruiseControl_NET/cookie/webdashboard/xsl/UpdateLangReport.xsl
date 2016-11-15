<?xml version="1.0"?>
<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:output method="html"/>
  
  <xsl:variable name="addCount" select="count(/cruisecontrol/build/maiet/updatelang/add/*)"/>
  <xsl:variable name="changeCount" select="count(/cruisecontrol/build/maiet/updatelang/change/*)"/>
  <xsl:variable name="debugCount" select="count(/cruisecontrol/build/maiet/updatelang/debug/*)"/>
    
  <xsl:template match="/">

  <xsl:if test="/cruisecontrol/build/maiet/updatelang">
	<table class="section-table" cellpadding="2" cellspacing="0" border="0" width="98%">

    <tr>
      <td class="sectionheader" colspan="2">문자열 테이블 갱신 결과</td>
    </tr>
    <tr>
      <td class="section-error" colspan="2">변경된 문자열: <xsl:value-of select="$changeCount"/></td>
    </tr>
    <tr>
       <td class="section-data" colspan="2">미번역 문자열: <xsl:value-of select="$addCount"/></td>
    </tr>
    <tr>
       <td class="section-data" colspan="2">개발용 문자열: <xsl:value-of select="$debugCount"/></td>
    </tr>
    

    <tr>
      <td class="sectionheader" colspan="2">문자열 테이블 갱신 결과: 변경 <xsl:value-of select="$changeCount"/></td>
    </tr>

    <xsl:if test="$changeCount > 0">
      <xsl:for-each select="/cruisecontrol/build/maiet/updatelang/change/*">
        <tr>
          <xsl:if test="position() mod 2=0">
            <xsl:attribute name="class">section-oddrow</xsl:attribute>
          </xsl:if>
          <xsl:if test="position() mod 2!=0">
            <xsl:attribute name="class">section-evenrow</xsl:attribute>
          </xsl:if>
          
          <td class="section-error">
            <xsl:value-of select="@key"/>
          </td>
          <td class="section-error">
            <xsl:value-of select="@string"/>
          </td>
        </tr>
      </xsl:for-each>
    </xsl:if>
    
    
    <tr>
        <td class="sectionheader" colspan="2">문자열 테이블 갱신 결과: 미번역 <xsl:value-of select="$addCount"/></td>
    </tr>

	  <xsl:if test="$addCount > 0">
      <xsl:for-each select="/cruisecontrol/build/maiet/updatelang/add/*">
        <tr>
          
          <xsl:if test="position() mod 2=0">
            <xsl:attribute name="class">section-oddrow</xsl:attribute>
          </xsl:if>
          <xsl:if test="position() mod 2!=0">
            <xsl:attribute name="class">section-evenrow</xsl:attribute>
          </xsl:if>
          
          <td class="section-data">
            <xsl:value-of select="@key"/>
          </td>
          <td class="section-data">
            <xsl:value-of select="@string"/>
          </td>
        </tr>
      </xsl:for-each>
	  </xsl:if>

    
    <tr>
      <td class="sectionheader" colspan="2">문자열 테이블 갱신 결과: 개발 중 <xsl:value-of select="$debugCount"/></td>
    </tr>

    <xsl:if test="$debugCount > 0">
      <xsl:for-each select="/cruisecontrol/build/maiet/updatelang/debug/*">
        <tr>
          <xsl:if test="position() mod 2=0">
            <xsl:attribute name="class">section-oddrow</xsl:attribute>
          </xsl:if>
          <xsl:if test="position() mod 2!=0">
            <xsl:attribute name="class">section-evenrow</xsl:attribute>
          </xsl:if>
          
          <td class="section-data">
            <xsl:value-of select="@key"/>
          </td>
          <td class="section-data">
            <xsl:value-of select="@string"/>
          </td>
        </tr>
      </xsl:for-each>
    </xsl:if>
    
  </table>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>
