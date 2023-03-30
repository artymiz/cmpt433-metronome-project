// https://progressivecoder.com/serve-static-html-files-in-nodejs-without-express/
// https://www.w3schools.com/nodejs/nodejs_uploadfiles.asp

const port = 3000;

//NodeJS Core Modules
const http = require('http');
const fs = require('fs');

//Third-Party Modules
const httpStatus = require('http-status-codes');
const formidable = require('formidable');

server = http.createServer((request, response) => {
    if (request.url === "/") {
        fs.readFile("index.html", (error, data) => {
            response.writeHead(httpStatus.StatusCodes.OK, {"Content-Type": "text/html"});
            response.write(data);
            response.end();
        })
    } else if (request.url === "/file-submit") {
        // Handle file submission
        const form = new formidable.IncomingForm()
        form.parse(request, (err, fields, files) => {
            const tmpFilePath = files['audio-sample'].filepath;
            // console.log(filename); // "/tmp/0e1f004f1355cfe6e54daa000" in the /tmp folder on my Linux file system
            fs.copyFile(tmpFilePath, "./uploaded/audio-sample.wav", (err) => {if (err) throw err});
        })
        // Redirect to homepage
        console.log("Successful file transfer");
        response.writeHead(302,  {Location: "/"})
        response.end();
    }
    else {
        response.writeHead(httpStatus.StatusCodes.NOT_FOUND, {"Content-Type": "text/html"});
        response.end("<h1>Sorry, page not found</h1>");
    }
})

server.listen(port);
console.log(`The server is listening on port: ${port}`);
