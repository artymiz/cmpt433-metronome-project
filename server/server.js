
const PORT_NUMBER = 8042

const http = require('http')
const fs = require('fs')
const path = require('path')
const mime = require('mime')

const server = http.createServer(function (request, response) {
    let filePath = ''
    if (request.url == '/') {
        filePath = 'public/index.html'
    } else {
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
