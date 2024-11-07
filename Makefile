ONAME = bot

main: 
	g++ ./src/main.cpp -o ./build/$(ONAME) -ldpp

clean:
	rm ./build/$(ONAME)
