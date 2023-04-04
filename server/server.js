
const PORT_NUMBER = 8042

// Node core modules
const http = require('http')
const fs = require('fs')
const path = require('path')

// 3rd party modules
const mime = require('mime')
const formidable = require('formidable')

// Function used for naming the next upload file
// based on the names of the files in uploads.  
// Valid upload file names are 0.wav ... 9.wav.
function getUploadName() {
    const maxUploads = 10 // Max number of files in uploads.
    const filenames = fs.readdirSync('uploads')
    filenameNumbers = filenames.map(elem => Number(elem[0])) // 'x.wav' => x
    for (let n = 0; n < maxUploads; n++) {
        if (!filenameNumbers.includes(n)) {
            return `${n}.wav`
        }
    }
    return '0.wav' // If no other options, overwrite first (oldest) sample.
}

const server = http.createServer(function (request, response) {
    let filePath = ''
    if (request.url == '/') {
        filePath = 'public/index.html'
    }
    else if (request.url === "/file-submit") {
        // Handle file submission
        const form = new formidable.IncomingForm()
        form.parse(request, (err, fields, files) => {
            const tmpFilePath = files['audio-sample'].filepath
            // console.log(filename); // "/tmp/0e1f004f1355cfe6e54daa000" in the /tmp folder on my Linux file system
            fs.copyFile(tmpFilePath, "./uploads/" + getUploadName(), (err) => { if (err) throw err })
        })
        // Redirect to homepage
        console.log("Successful file transfer")
        response.writeHead(302, { Location: "/" })
        response.end()
    }
    else {
        filePath = 'public' + request.url
    }
    serveStatic(response, './' + filePath)
})

server.listen(PORT_NUMBER, function () {
    console.log("Server listening on port " + PORT_NUMBER)
})

function serveStatic(response, absPath) {
    fs.exists(absPath, function (exists) {
        if (exists) {
            fs.readFile(absPath, function (err, data) {
                if (err) {
                    send404(response)
                } else {
                    sendFile(response, absPath, data)
                }
            })
        } else {
            send404(response)
        }
    })
}

function send404(response) {
    response.writeHead(404, { 'Content-Type': 'text/plain' })
    response.write('Error 404: resource not found.')
    response.end()
}

function sendFile(response, filePath, fileContents) {
    response.writeHead(
        200,
        { "content-type": mime.lookup(path.basename(filePath)) }
    )
    response.end(fileContents)
}

const udpMechanism = require('./udp-mechanism')
udpMechanism.listen(server)
