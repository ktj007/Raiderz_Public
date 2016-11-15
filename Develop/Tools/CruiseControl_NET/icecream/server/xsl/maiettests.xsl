<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/TR/xhtml1/strict">
	<xsl:output method="html"/>
	<xsl:template match="/">
		<xsl:variable name="maiettest.result.list" select="/cruisecontrol/build/maiettest-results"/>
		<xsl:variable name="maiettest.report.list" select="$maiettest.result.list/report"/>
		<xsl:variable name="maiettest.case.list" select="$maiettest.result.list/test"/>

		<table class="section-table" cellpadding="2" cellspacing="0" border="0" width="98%">
			<tr>
				<td class="sectionheader">
					Maiet Test report:
				</td>
			</tr>
			<xsl:call-template name="maiettest__reportdetail">
				<xsl:with-param name="detailnodes" select="//report"/>
			</xsl:call-template>
		</table>

		<table class="section-table" cellpadding="2" cellspacing="0" border="0" width="98%">
			<tr>
				<td class="sectionheader" colspan="2">
					Maiet Test Case results:
				</td>
			</tr>
			<xsl:call-template name="maiettest__testdetail">
				<xsl:with-param name="detailnodes" select="//test"/>
			</xsl:call-template>
		</table>
	</xsl:template>

	<!-- Maiettest Report Detail Template -->
	<xsl:template name="maiettest__reportdetail">
		<xsl:param name="detailnodes"/>

		<xsl:for-each select="$detailnodes">
			<tr>
				<td >
					<xsl:value-of select="@text"/>
				</td>
			</tr>

		</xsl:for-each>
	</xsl:template>

	<!-- Maiettest TestCase Detail Template -->
	<xsl:template name="maiettest__testdetail">
		<xsl:param name="detailnodes"/>

		<xsl:for-each select="$detailnodes">


			<tr>
				<td class="section-data" width="100">Test:</td>
				<td class="section-data">
					<xsl:value-of select="@name"/>
				</td>
			</tr>
			<tr>
				<td class="section-data">Time:</td>
				<td class="section-data">
					<xsl:value-of select="@time"/>
				</td>
			</tr>

			<xsl:if test="failure">
				<tr>
					<td class="section-data">Type:</td>
					<td class="section-data">
						<span style="color: red;">Failure</span>
					</td>
				</tr>
			</xsl:if>
			<xsl:if test="success">
				<tr>
					<td class="section-data">Type:</td>
					<td class="section-data">
						<span style="color: blue;">Success</span>
					</td>
				</tr>
			</xsl:if>

			<xsl:for-each select="failure">
				<tr>
					<td class="section-data">Message:</td>
					<td class="section-data">
						<xsl:value-of select="@message"/>
					</td>
				</tr>
			</xsl:for-each>
			<xsl:for-each select="success">
				<tr>
					<td class="section-data">Message:</td>
					<td class="section-data">
						<xsl:value-of select="@message"/>
					</td>
				</tr>
			</xsl:for-each>


			<tr>
				<td colspan="2">
					<hr size="1" width="100%" color="#888888"/>
				</td>
			</tr>

		</xsl:for-each>
	</xsl:template>

</xsl:stylesheet>
