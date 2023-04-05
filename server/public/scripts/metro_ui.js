
var socket = io.connect()
const tempoInputElement = document.getElementById('tempoval')
// const MINTEMPO
// const MAXTEMPO

socket.on('command-reply', function(reply) {
    $('#status').text("Online");
    $('#status').css('color', 'green');
    const [type, value] = reply.split(' ')
    switch (type) {
        case 'tempo':
            $('#tempoval').text(value);
            //tempoInputElement.value = parseInt(value)
            break
        case 'time':
            let time_val = parseInt(value)
            document.getElementById('status_time').innerText = 'device uptime:\n ' + time_convert(time_val) + '  (H:M:S)'
            break
    }
})

$('#tempo-decrease-btn').click(() => {
        // socket.emit('command-send'...) has corresponding socket.on('command-send', (arg) => ...) in udp-mechanism.js
        socket.emit('command-send', 'tempodec')
})

$('#tempo-increase-btn').click(() => {
        socket.emit('command-send', 'tempoinc')
})

const time_convert = (seconds) => {
    seconds = parseInt(seconds)
    let hours = seconds / 3600
    seconds = seconds % 3600
    let mins = seconds / 60
    seconds = seconds % 60
    return parseInt(hours) + ' : ' + parseInt(mins) + ' : ' + (seconds)
}

$(document).ready(function () {
    window.setInterval(() => {
        socket.emit('command-send', 'gettempo')
    }, 1000)
})
