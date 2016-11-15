<?xml version="1.0"?>
<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:output method="html"/>

  <xsl:variable name="addCount" select="count(/cruisecontrol/build/maiet/updatelang/add)"/>
  <xsl:variable name="changeCount" select="count(/cruisecontrol/build/maiet/updatelang/change)"/>
    
  <xsl:template match="/">

    <table class="section-table" cellpadding="2" cellspacing="0" border="0" width="98%">
      <tr>
        <td class="sectionheader" colspan="2">문자열 테이블 갱신 결과</td>
      </tr>
      <tr>
        <td colspan="2" class="section-data">
          추가: <xsl:value-of select="$addCount"/>
        </td>
      </tr>
      <tr>
        <td colspan="2" class="section-data">
          변경: <xsl:vaule-of select="$changeCount"/>
        </td>
      </tr>


      <tr>
        <td class="sectionheader" colspan="2">추가 문자열 목록</td>
      </tr>

      <xsl:for-each select="/cruisecontrol/build/maiet/updatelang/add">
        <tr>
          <td class="section-data">
            <xsl:value-of select="@key"/>
          </td>
          <td class="section-data">
            <xsl:value-of select="@string"/>
          </td>
        </tr>
      </xsl:for-each>

      <tr>
        <td class="sectionheader" colspan="2">변경 문자열 목록</td>
      </tr>
      <xsl:for-each select="/cruisecontrol/build/maiet/updatelang/change">
        <tr>
          <td class="section-error">
            <xsl:value-of select="@key"/>
          </td>
          <td class="section-error">
            <xsl:value-of select="@string"/>
          </td>
        </tr>
      </xsl:for-each>
    
    </table>
    
  </xsl:template>

</xsl:stylesheet>
