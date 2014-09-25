%%
%% testsample.m
%%

disp(date);

cd "C:\\octave\\Octave3.6.2_gcc4.6.2\\bin"
procid = popen("octave C:\\Eugene\\testsofgnuplotandoctave\\logtest6.m","r");
m = fopen("c:\\Eugene\\testsofgnuplotandoctave\\debug.txt","w");
do
	s = fgets(procid); 
	fputs(m,s); 
until (ischar(s) == 0);
fclose(m);
fclose(procid);
cd "c:\\eugene\\testsofgnuplotandoctave"