for f in video/other8/*
do
	./process_video -dd --sw-config sw_config.xml -i "11" --db -n -v $f 
done