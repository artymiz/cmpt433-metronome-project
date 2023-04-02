var connection_status = false
var socket = io.connect()
const tempoInputElement = document.getElementById('tempoval')
// const MINTEMPO
// const MAXTEMPO

socket.on('command-reply', (reply) => {
    if (!connection_status) {
        connection_status = !connection_status
        $('#online_status')[0].style.color = 'green'
        $('#online_status')[0].innerText = 'online'
        $('#status')[0].hidden = false
    }
    const [type, value] = reply.split(' ')
    switch (type) {
        case 'tempo':
            tempoInputElement.value = parseInt(value)
            break
        case 'time':
            let time_val = parseInt(value)
            document.getElementById('status_time').innerText = 'device uptime:\n ' + time_convert(time_val) + '  (H:M:S)'
            break
    }
})

$('#tempoDecrease').click(() => {
    if (connection_status) {
        let val = parseInt(tempoInputElement.value)
        val = val - 5 > 40 ? val - 5 : 40
        // socket.emit('command-send'...) has corresponding socket.on('command-send', (arg) => ...) in udp-mechanism.js
        socket.emit('command-send', 'tempo ' + val + '\n')
    }
})

$('#tempoIncrease').click(() => {
    if (connection_status) {
        let val = parseInt(tempoInputElement.value)
        val = val + 5 < 200 ? val + 5 : 200
        socket.emit('command-send', 'tempo ' + val + '\n')
    }
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
        socket.emit('command-send', 'uptime\n')
    }, 1000)
})
