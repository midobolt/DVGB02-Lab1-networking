# DVGB02-Lab1-networking
C networking lab – part 1: web server


## Description

This lab implements a simple **web server in C** that serves files from the folder `sample_website`.  
It supports:

- HTTP GET requests  
- Default `index.html`  
- Serving static files (HTML, CSS, JPG, PNG)  
- 404 response for missing files  

---

## Files

- `web_server.c` – Web server source code  
- `sample_website/` – Folder with `index.html` and other files  

---

## Compile & Run

```bash
gcc web_server.c -o web_server
./server

Server listens on port 8080
Access in browser: http://localhost:8080/