# the goal

Server app (Crow.h), with web interface (HTMX), generating health parameter graphs (LodePNG) from user input


# the tech stack

1. front-end htmx

2. c++ backend with libraries 
	http server
		- process requests
		- serve html back to web ui
		- store cookie for user id to fingerprint, and store a copy in database to see
		(create test cookie that has preloaded data in it)
	generate png
		- graph the data
	sqlite
		- data base

*HTTP Server*: [Crow](https://github.com/CrowCpp/Crow)
*PNG Generation*: [LodePNG](https://github.com/lvandeve/lodepng)






