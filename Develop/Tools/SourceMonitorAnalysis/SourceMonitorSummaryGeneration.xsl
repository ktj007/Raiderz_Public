<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  
  <!--- SourceMonitor Summary Xml File Generation created by Eden Ridgway -->
  <xsl:output method="xml"/>
  
  <xsl:template match="/">
    <xsl:apply-templates select="/sourcemonitor_metrics" />
  </xsl:template>
  
  <!-- Transform the results into a simpler more intuitive and summarised format -->
  <xsl:template match="sourcemonitor_metrics">
      <SourceMonitorComplexitySummary>
        <MostComplexMethods>
          <xsl:call-template name="MostComplexMethods"/>
        </MostComplexMethods>
        
        <MostDeeplyNestedCode>
          <xsl:call-template name="MostDeeplyNestedCode"/>
        </MostDeeplyNestedCode>
        
      </SourceMonitorComplexitySummary>
  </xsl:template>

  <!-- Complexity Metrics -->
  <xsl:template name="MostComplexMethods">
    <xsl:for-each select=".//file">
      <xsl:sort select="metrics/metric[@id='M9']" order="descending" data-type="number" />
      
      <xsl:choose>
        <xsl:when test="position() &lt; 101">
           <Method>
              <File><xsl:value-of select="@file_name"/></File>
              <Name><xsl:value-of select="metrics/metric[@id='M8']"/></Name>
              <Line><xsl:value-of select="metrics/metric[@id='M10']"/></Line>
              <Complexity><xsl:value-of select="metrics/metric[@id='M9']"/></Complexity>
			  <Statements><xsl:value-of select="metrics/metric[@id='M1']"/></Statements>
			  <MaximumDepth><xsl:value-of select="metrics/metric[@id='M11']"/></MaximumDepth>
			  <AverageComplexity><xsl:value-of select="metrics/metric[@id='M13']"/></AverageComplexity>
           </Method>
        </xsl:when>
      </xsl:choose>
    </xsl:for-each>
  </xsl:template>

  <!-- Nesting Metrics -->
  <xsl:template name="MostDeeplyNestedCode">
    <xsl:for-each select=".//file">
        <xsl:sort select="metrics/metric[@id='M11']" order="descending" data-type="text" />
      
        <xsl:choose>
          <xsl:when test="position() &lt; 16">
             <Block>
                <File><xsl:value-of select="@file_name"/></File>
                <Depth><xsl:value-of select="metrics/metric[@id='M11']"/></Depth>
                <Line><xsl:value-of select="metrics/metric[@id='M10']"/></Line>
             </Block>
          </xsl:when>
        </xsl:choose>
    </xsl:for-each>
  </xsl:template>
  
</xsl:stylesheet>
