
var _numberRecentGraphXTicks=4;
var _numberHistoricGraphXTicks=4;
var _shouldDisplayGraphsAsTimelines=false;
var _recentGraphConfigurations=
[
	{
		graphName:"Build Duration",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"Build Duration (seconds)",
				attributeName:"DurationInSeconds",
				color:"green"
			}
		]
	},
	{
		graphName:"Tests",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"Passed",
				attributeName:"TestsPassed",
				color:"green"
			},
			{
				name:"Failed",
				attributeName:"TestFailures",
				color:"red"
			},
			{
				name:"Ignored",
				attributeName:"TestIgnored",
				color:"yellow"
			}
		]
	},
	{
		graphName:"Test Coverage",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		yRange:{lower:0,upper:100},
		dataType:'decimal',
		series:
		[
			{
				name:"Coverage",
				attributeName:"Coverage",
				color:"blue"
			}
		]
	},
	{
		graphName:"Gendarme",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"Gendarme Defects",
				attributeName:"GendarmeDefects",
				color:"red"
			}
		]
	},
	{
		graphName:"FxCop",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"Errors",
				attributeName:"FxCop Errors",
				color:"red"
			},
			{
				name:"Warnings",
				attributeName:"FxCop Warnings",
				color:"yellow"
			}
		]
	},
	{
		graphName:"Statements",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"Statements",
				attributeName:"Statements",
				color:"blue"
			}
		]
	},
	{
		graphName:"Complexity",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"Average Complexity",
				attributeName:"AverageComplexity",
				color:"blue"
			}
		]
	},
	{
		graphName:"FPS",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"FPS Average",
				attributeName:"FPSAverage",
				color:"green"
			},
			{
				name:"FPS Min",
				attributeName:"FPSMin",
				color:"red"
			},
			{
				name:"FPS Max",
				attributeName:"FPSMax",
				color:"blue"
			}
		]
	},
	{
		graphName:"Number of calls",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"Number of calls average",
				attributeName:"NumberOfCallsAverage",
				color:"green"
			}
		]
	},
	{
		graphName:"Number of draw calls",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"Number of draw calls average",
				attributeName:"NumberOfDrawCallsAverage",
				color:"green"
			}
		]
	},
	{
		graphName:"Time spent in draw calls",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"Time spent in draw calls average",
				attributeName:"TimeSpentInDrawCallsAverage",
				color:"green"
			}
		]
	},
	{
		graphName:"Locks on resource objects",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"Locks on resource objects average",
				attributeName:"LocksOnResourceObjectsAverage",
				color:"green"
			}
		]
	},
	{
		graphName:"Unlocks on resource objects",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"Unlocks on resource objects average",
				attributeName:"UnlocksOnResourceObjectsAverage",
				color:"green"
			}
		]
	},
	{
		graphName:"SetRenderState calls",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"SetRenderState calls average",
				attributeName:"SetRenderStateCallsAverage",
				color:"green"
			}
		]
	},
	{
		graphName:"SetRenderTarget calls",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"SetRenderTarget calls average",
				attributeName:"SetRenderTargetCallsAverage",
				color:"green"
			}
		]
	},
	{
		graphName:"SetVertexShader calls",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"SetVertexShader calls average",
				attributeName:"SetVertexShaderCallsAverage",
				color:"green"
			}
		]
	},
	{
		graphName:"SetPixelShader calls",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"SetPixelShader calls average",
				attributeName:"SetPixelShaderCallsAverage",
				color:"green"
			}
		]
	},
	{
		graphName:"SetTexture calls",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"SetTexture calls average",
				attributeName:"SetTextureCallsAverage",
				color:"green"
			}
		]
	},
	{
		graphName:"SetVertexShaderConstant calls",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"SetVertexShaderConstant calls average",
				attributeName:"SetVertexShaderConstantCallsAverage",
				color:"green"
			}
		]
	},
	{
		graphName:"SetPixelShaderConstant calls",
		dataSource:_recentStatistics,
		numXTicks:_numberRecentGraphXTicks,
		series:
		[
			{
				name:"SetPixelShaderConstant calls average",
				attributeName:"SetPixelShaderConstantCallsAverage",
				color:"green"
			}
		]
	}
];

var _summaryConfiguration=
{
	successfulBuildCount:function(successfulBuilds,failedBuilds){return count(successfulBuilds)},
	failedBuildCount:function(successfulBuilds,failedBuilds){return count(failedBuilds)},
	averageBuildDuration:function(successfulBuilds,failedBuilds){return average(successfulBuilds,"DurationInSeconds")
},

minBuildDuration:function(successfulBuilds,failedBuilds){return min(successfulBuilds,"DurationInSeconds")},
maxBuildDuration:function(successfulBuilds,failedBuilds){return max(successfulBuilds,"DurationInSeconds")},
averageCoverage:function(successfulBuilds,failedBuilds){return average(successfulBuilds,"Coverage")},
testCount:function(successfulBuilds,failedBuilds){return average(successfulBuilds,"TestCount")},
testsPassed:function(successfulBuilds,failedBuilds){return average(successfulBuilds,"TestsPassed")},
testFailures:function(successfulBuilds,failedBuilds){return average(successfulBuilds,"TestFailures")},
testsIgnored:function(successfulBuilds,failedBuilds){return average(successfulBuilds,"TestIgnored")},
gendarmeDefects:function(successfulBuilds,failedBuilds){return average(successfulBuilds,"GendarmeDefects")},
fxCopWarnings:function(successfulBuilds,failedBuilds){return average(successfulBuilds,"FxCop Warnings")},
fxCopErrors:function(successfulBuilds,failedBuilds){return average(successfulBuilds,"FxCop Errors")},
averageStatements:function(successfulBuilds,failedBuilds){return average(successfulBuilds,"Statements")},
averageComplexity:function(successfulBuilds,failedBuilds){return average(successfulBuilds,"AverageComplexity")}};

var _historicGraphConfigurations=
[
	{
		graphName:"Build Report",
		dataSource:_summarisedStatistics,
		numXTicks:_numberHistoricGraphXTicks,
		series:
		[
			{
				name:"Successful Builds",
				attributeName:"successfulBuildCount",
				color:"green"
			},
			{
				name:"Failed Builds",
				attributeName:"failedBuildCount",
				color:"red"
			}
		]
	},
	{
		graphName:"Build Duration",
		dataSource:_summarisedStatistics,
		numXTicks:_numberHistoricGraphXTicks,
		series:
		[
			{
				name:"Max Duration (seconds)",
				attributeName:"maxBuildDuration",
				color:"blue"
			},
			{
				name:"Average Duration (seconds)",
				attributeName:"averageBuildDuration",
				color:"green"
			}
		]
	},
	{
		graphName:"Test Summary",
		dataSource:_summarisedStatistics,
		numXTicks:_numberHistoricGraphXTicks,
		series:
		[
			{
				name:"Average Tests Passed",
				attributeName:"testsPassed",
				color:"green"
			},
			{
				name:"Average Test Failures",
				attributeName:"testFailures",
				color:"red"
			},
			{
				name:"Average Tests Ignored",
				attributeName:"testsIgnored",
				color:"yellow"
			}
		]
	},
	{
		graphName:"Test Coverage",
		dataSource:_summarisedStatistics,
		numXTicks:_numberHistoricGraphXTicks,
		dataType:'decimal',
		yRange:{lower:0,upper:100},
		series:
		[
			{
				name:"Coverage",
				attributeName:"averageCoverage",
				color:"blue"
			}
		]
	},
	{
		graphName:"Gendarme",
		dataSource:_summarisedStatistics,
		numXTicks:_numberHistoricGraphXTicks,
		series:
		[
			{
				name:"Average Defects",
				attributeName:"gendarmeDefects",
				color:"red"
			}
		]
	},
	{
		graphName:"FxCop Errors/Warnings",
		dataSource:_summarisedStatistics,
		numXTicks:_numberHistoricGraphXTicks,
		series:
		[
			{
				name:"Average Warnings",
				attributeName:"fxCopWarnings",
				color:"yellow"
			},
			{
				name:"Average Errors",
				attributeName:"fxCopErrors",
				color:"red"
			}
		]
	},
	{
		graphName:"Statements",
		dataSource:_summarisedStatistics,
		numXTicks:_numberHistoricGraphXTicks,
		series:
		[
			{
				name:"Average Statements",
				attributeName:"averageStatements",
				color:"blue"
			}
		]
	},
	{
		graphName:"Complexity",
		dataSource:_summarisedStatistics,
		numXTicks:_numberHistoricGraphXTicks,
		series:
		[
			{
				name:"Average Complexity",
				attributeName:"averageComplexity",
				color:"blue"
			}
		]
	}
];