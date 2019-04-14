all: ttweetsrv ttweetcli

ttweetsrv: ./server/ttweetsrv.c ./dependencies/ttweet_common.c ./dependencies/cJSON.c
	gcc ./server/ttweetsrv.c ./dependencies/ttweet_common.c ./dependencies/cJSON.c -o ttweetsrv

ttweetcli: ./client/ttweetcli.c ./dependencies/ttweet_common.c ./dependencies/cJSON.c
	gcc ./client/ttweetcli.c ./dependencies/ttweet_common.c ./dependencies/cJSON.c -o ttweetcli