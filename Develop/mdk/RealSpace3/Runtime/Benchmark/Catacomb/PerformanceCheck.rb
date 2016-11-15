require "csv"
require "rexml/document"

itemArray = ["Duration", "FPS", "Number of Calls", "Number of Draw* calls", "Time spent in Draw* calls", "Locks on resource objects this frame", "Unlocks on resource objects this frame", "SetRenderState calls this frame", "SetRenderTarget calls this frame", "SetVertexShader calls this frame", "SetPixelShader calls this frame", "SetTexture calls this frame", "SetVertexShaderConstant<T> calls this frame", "SetPixelShaderConstant<T> calls this frame"]
itemIndexArray = Array.new
itemSumArray = Array.new

count = 0;
CSV.foreach( "PerformanceCheck.csv" ) do |row|
	if( count == 0 )
		itemArray.each do |item|
			index = row.index item
			itemIndexArray.push( index )
			itemSumArray.push( Array.new );
		end
	#	p itemIndexArray
	else
		itemIndexArray.each_with_index do |index, i|
			value = row[index].to_f
			itemSumArray[i].push( value )
		end
	end
	
	count += 1
end

# remove empty value
itemSumArray.each do |arr|
	arr.reject! { |i| i == 0 }
end

xmlDoc = REXML::Document.new
elStatistics = xmlDoc.add_element "PERFSTAT"

elItem = elStatistics.add_element "TIME"
time = Time.new
time.inspect
elItem.attributes["year"] = time.year
elItem.attributes["month"] = time.month
elItem.attributes["day"] = time.day
elItem.attributes["hour"] = time.hour
elItem.attributes["min"] = time.min
elItem.attributes["sec"] = time.sec

itemSumArray.each_with_index do |arr, i|
	total = 0
	avg = 0
	arr.each { |value| total += value }
	avg = total / arr.size
	
	itemName = itemArray[i]
	
	elItem = elStatistics.add_element "STAT"
	elItem.attributes["name"] = itemName
	elItem.attributes["min"] = arr.min.to_s
	elItem.attributes["max"] = arr.max.to_s
	elItem.attributes["avg"] = avg.to_s

#	p ( itemArray[i] + " Min: " + arr.min.to_s + " Max: " + arr.max.to_s + " Avg: " + avg.to_s )
end

file = File.new( "StatisticsResult.xml", "w" )
xmlDoc.write( file, 4 )
file.close

file = File.new( "StatisticsResultHistory.xml", "a" )
xmlDoc.write( file, 4 )
file.close