1;   % a dummy command needed to assure Octave that this file was not
     % intended as a function file accomodating function timeinseconds();
     % for more information see "GNU Octave. Free Your Numbers", 
     % chapter 11 "Functions and Scripts" -> "11.9 Script Files"

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%
%%%% this function receives its input argument in the format
%%%% "yyyy-mm-dd HH:MM:SS,FFF". The function converts the argument
%%%% to format "yyyy-mm-dd HH:MM:SS.FFF" (replaces the comma  with 
%%%% the dot) and then converts and returns to the calling routine
%%%% that value in expressed in seconds; 
%%%% (history of the function design can be traced in the file
%%%% timedif.m)
function timeinsec = timeinseconds(datetime_as_string)

	disp("time in seconds");
	
	disp(datetime_as_string);
	
	%% implementation number one
	[head, tail] = strtok(datetime_as_string,",");
	workstring = sprintf("%s.%s", head, tail(2:4));
	
	%%%% implementation number two (an alternative to implementation number one above)
	%%%% left here intentionally for exploratory tests
	%~ matrix_of_two_strings = strsplit(datetime_as_string,",");
	%~ workstring = sprintf("%s.%s",matrix_of_two_strings{1},matrix_of_two_strings{2});
			
	[~, timeinsec] = datenum(workstring,"yyyy-mm-dd HH:MM:SS.FFF")
	
	timeinsec
	
endfunction
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
try

directory_name = "c:\\Eugene\\testsofgnuplotandoctave\\logs\\";
alldata = {};

allfiles = dir (strcat(directory_name, "*.txt"));

number_of_files = numel(allfiles);

n = 0;
for i = 1:number_of_files					% loop through all files in the folder
	
	filename = allfiles(i).name

	fileForRetrieveID = fopen(sprintf("%s%s",directory_name,filename),"r");

	disp(fileForRetrieveID);

	parse_begin = false;
	while (!feof(fileForRetrieveID))
		current_line = fgets(fileForRetrieveID);
		if (!parse_begin) && (! isempty(findstr(current_line, "work item")))
			%disp(current_line);
			x.started_at = strtok(current_line, "DEBUG");
			x.typeofscan = substr(current_line,rindex(current_line,":") + 2);
			parse_begin = true;
		else
			if (parse_begin) && (! isempty(findstr(current_line, "Closing database connection")))
				x.ended_at = strtok(current_line, "DEBUG");
				n = n + 1;
				alldata(n) = x;
				parse_begin = false;
			endif
		endif
	endwhile

	fclose(fileForRetrieveID);
endfor								% end of loop through all files in the folder

types_used = [];
for i = 1:numel(alldata)
	disp(sprintf("date and time: %s, end of save: %s, type of scan: %s", alldata{i}.started_at, alldata{i}.ended_at, alldata{i}.typeofscan))
	types_used {i} = alldata{i}.typeofscan;
endfor

types_used                        # for debugging only; to be commented out eventually
ttt = unique(types_used)          # ttt is a "no duplicates" list of types of scans




data_sorted = {}
for currentAnalyzedTypeOfScan = ttt                                             
	transientDataHolder.nameOfGivenTypeOfScan = currentAnalyzedTypeOfScan;        
	transientDataHolder.currentAverage = 0.0;

	# second element of the vector returned by 
	# function "ismember" is not currently used;
	# m (vector) holds TRUE (1)  for each record in
	# types_used where the type equals to
	# currentAnalyzedTypeOfScan thus "TRUE" entries
	# of m highlight entries of interest in alldata
	
	[m,~] = ismember(types_used,currentAnalyzedTypeOfScan);                 
	timingSamples = {};                                                     
	for kk = 1:numel(m)                                                     
		if m(kk) == true                                               
			varTimeframe.begin = alldata{kk}.started_at;            
			varTimeframe.end = alldata{kk}.ended_at;
			%~ varTimeframe.duration = timedif(varTimeframe.begin, varTimeframe.end); 
			varTimeframe.duration = timeinseconds(varTimeframe.end) - timeinseconds(varTimeframe.begin);
			currentNumberOfSamples = numel(timingSamples) + 1;
			%~ timingSamples(numel(timingSamples)+1) = varTimeframe;
			timingSamples(currentNumberOfSamples) = varTimeframe;
			
			%% recursive calculation of mean values
			transientDataHolder.currentAverage = (1/currentNumberOfSamples)* ...
				(transientDataHolder.currentAverage * (currentNumberOfSamples - 1) +  varTimeframe.duration);
			
			%% recursive calculation of standard deviation
			if currentNumberOfSamples < 2
				transientDataHolder.currentVariance = 0;
				transientDataHolder.currentStandardDeviation = 0;
			else
				preVariance = (currentNumberOfSamples - 2) * transientDataHolder.currentVariance + ...
					(currentNumberOfSamples * ((varTimeframe.duration - transientDataHolder.currentAverage)**2)/ ...
					(currentNumberOfSamples - 1));
				transientDataHolder.currentVariance = preVariance /(currentNumberOfSamples - 1);
				transientDataHolder.currentStandardDeviation = sqrt(transientDataHolder.currentVariance);
			endif
		endif
	endfor
	transientDataHolder.numberOfSamples = currentNumberOfSamples;       % 11212012
	transientDataHolder.timesOfGivenTypeOfScan = timingSamples;
	data_sorted(numel(data_sorted)+1) = transientDataHolder;
endfor

data_sorted

%% prepare output file name:  
%%                            (1) prepare fixed part of the name, 
%%                            (2) prepare timestamp as distinctive suffix
%%                            (3) attach the suffix to the fixed part

%% (1)
output_file_base = "c:\\eugene\\testsofgnuplotandoctave\\report";

%% (2)
int_time = floor(clock);
time_suffix = "";
for i = 1:numel(int_time)
	if int_time(i) > 9                                               %% padding with 0 the values 
		time_suffix = sprintf("%s%d",time_suffix,int_time(i));   %% less than 10 for uniform 
	else                                                             %% alignment of the timestamp
		time_suffix = sprintf("%s0%d",time_suffix,int_time(i));
	endif
endfor

%% (3)
output_file_used = strcat(output_file_base, "_",time_suffix, ".txt");

%% open it
current_fid = fopen(output_file_used,"a");
fprintf(current_fid, "\r\n *************\r\n ************* Raw Results ************* \r\n *************\r\n\r\n" ) ;

%% write in 
for i = 1:numel(data_sorted)
	fprintf(current_fid, " Type of scan: %s", data_sorted{i}.nameOfGivenTypeOfScan{[1]});
	ss = sprintf("Number of Samples : %d", data_sorted{i}.numberOfSamples);
	fprintf(current_fid, "\t%s\r\n", ss);
	fprintf(current_fid, "\t%s", "The samples (seconds) : ");
	for j = 1: data_sorted{i}.numberOfSamples
		fprintf(current_fid, "%3.3f", data_sorted{i}.timesOfGivenTypeOfScan{j}.duration);
		if (j < data_sorted{i}.numberOfSamples)
			fprintf(current_fid,"%s",", ");	
		endif
	endfor
	fprintf(current_fid, "\r\n");
	
	%% average (mean) and standard deviation in case there are more than one sample
	if data_sorted{i}.numberOfSamples > 1                                                            
		fprintf(current_fid, "\t%s%3.3f", "The average (seconds): ",data_sorted{i}.currentAverage);
		fprintf(current_fid, "\r\n");
		fprintf(current_fid, "\t%s%3.3f", "Standard deviation (seconds): ",data_sorted{i}.currentStandardDeviation);
	endif
	fprintf(current_fid, "\r\n\r\n");
endfor

%% close
fclose(current_fid);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%% secondary processing
current_fid = fopen(output_file_used,"a+");
	fprintf(current_fid, "\r\n *************\r\n ************* Processed Results ************* \r\n *************\r\n\r\n" ) ;
fclose(current_fid);

for i = 1:numel(data_sorted)
	current_scan_data = sprintf("%s: ",data_sorted{i}.nameOfGivenTypeOfScan{[1]})     %% for debugging only ?!?!?
	current_matrix = [];
	for j = 1:data_sorted{i}.numberOfSamples
		current_scan_data = strcat(current_scan_data, sprintf("%3.3f, ", data_sorted{i}.timesOfGivenTypeOfScan{j}.duration));
		current_matrix(j) = data_sorted{i}.timesOfGivenTypeOfScan{j}.duration;
	endfor
	
	%% current_scan_data
	current_matrix
	%~ if length(current_matrix) >= 6
		%~ hist(current_matrix)
	%~ endif
	
	% a relic below
	% processed_matrix = statistics(current_matrix)
	
	%% outliers can be meaningfully identified in
	%% a "large enough" set of measurements 
	if numel(current_matrix) > 3 
	
		%% the set is "large enough": z_score shall be
		%% used to pinpoint the outliers
		
		z_matrix = abs(zscore(current_matrix))
		
		upper_quantile_of_z = quantile(z_matrix(:), [0.75])
		
		% the statement above determines only one quantile boundary
		% (by virtue of its value -- 0.75 -- it is the upper quartile boundary)
		% hence the expression below could also be edited as 
		%
		%		matrix_without_outliers = current_matrix(find(z_matrix < upper_quantile_of_z))
		%
		% the actual edition below is left for a probable change in utilization
		% of "quantile(...)" function above toward the use of multiple quantiles (quartiles); 
		% see also script "retrieved_matrix.m"
		matrix_without_outliers = current_matrix(find(z_matrix < upper_quantile_of_z(numel(upper_quantile_of_z))))
	else
		matrix_without_outliers = current_matrix
	endif
	
	mean_value_of_raw_data = mean(current_matrix); 
	mean_value_of_processed_data = mean(matrix_without_outliers);
	standard_deviation_of_raw_data = std(current_matrix);
	standard_deviation_of_processed_data = std(matrix_without_outliers);
	expected_improvement = standard_deviation_of_raw_data -standard_deviation_of_processed_data;
	
	result_of_processing = sprintf("Mean value of raw data: %3.3f \r\n",mean_value_of_raw_data);
	result_of_processing = strcat(result_of_processing, sprintf("Mean value of processed data: %3.3f \r\n",mean_value_of_processed_data));
	result_of_processing = strcat(result_of_processing, sprintf("Standard deviation of raw data: %3.3f \r\n",standard_deviation_of_raw_data));
	result_of_processing = strcat(result_of_processing, sprintf("Standard deviation of processed data: %3.3f \r\n",standard_deviation_of_processed_data));
	result_of_processing = strcat(result_of_processing, sprintf("Expected improvement: %3.3f \r\n",expected_improvement));
	disp(result_of_processing);
	
	%% logging the processed data in a file
	%% open it
	current_fid = fopen(output_file_used,"a+");
	fprintf(current_fid, " Type of scan: %s",data_sorted{i}.nameOfGivenTypeOfScan{[1]});
	fprintf(current_fid, "  \t%s", "The samples (seconds) : ");
	for j = 1: numel(current_matrix)
		fprintf(current_fid, "%3.3f", current_matrix(j));
		if (j < numel(current_matrix))
			fprintf(current_fid,"%s",", ");	
		endif
	endfor
	fprintf(current_fid, "\r\n");
	fprintf(current_fid, "  \t%s", "The filtered samples (seconds) : ");
	for j = 1: numel(matrix_without_outliers)
		fprintf(current_fid, "%3.3f", matrix_without_outliers(j));
		if (j < numel(matrix_without_outliers))
			fprintf(current_fid,"%s",", ");	
		endif
	endfor
	fprintf(current_fid, "\r\n");
	fprintf(current_fid, "\t%s%3.3f\r\n", "The average (seconds): ",mean_value_of_processed_data);
	fprintf(current_fid, "\t%s%3.3f\r\n", "Standard deviation (seconds): ",standard_deviation_of_processed_data);
	fprintf(current_fid, "\t%s%3.3f, %3.3f %s", "Expected improvement due to processing: ",expected_improvement, (expected_improvement/standard_deviation_of_raw_data)*100,"%");
	fprintf(current_fid, "\r\n\r\n");
	
	fclose(current_fid);
		
	
endfor

catch
	fclose(all);
end_try_catch

displayed_report_name = sprintf("\r\n ***\r\n *** Report saved in : %s \r\n ***\r\n", output_file_used);
disp(displayed_report_name);



