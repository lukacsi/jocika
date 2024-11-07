ONAME = bot

main: 
	g++ ./src/main.cpp -o ./build/$(ONAME) -ldpp -lmpg123

clean:
	rm ./build/$(ONAME)
