// https://progressivecoder.com/serve-static-html-files-in-nodejs-without-express/

const port = 3000;

//NodeJS Core Modules
const http = require('http');
const fs = require('fs');

//Third-Party Modules
const httpStatus = require('http-status-codes');

const routeMap = {
    "/": "index.html",
    "file-submit": "file-submit.html"
};

server = http.createServer((request, response) => {
    console.log(request.url)
    if (routeMap[request.url]) {
        fs.readFile(routeMap[request.url], (error, data) => {
            response.writeHead(httpStatus.StatusCodes.OK, {"Content-Type": "text/html"});
            response.write(data);
            response.end();
        })
    } else {
        response.writeHead(httpStatus.StatusCodes.NOT_FOUND, {"Content-Type": "text/html"});
        response.end("<h1>Sorry, page not found</h1>");
    }
})

server.listen(port);
console.log(`The server is listening on port: ${port}`);
