var connection_status = false
var socket = io.connect();
const tempo_ele = document.getElementById("tempoval");

socket.on('commandReply',(result) =>{
    if(!connection_status){
        connection_status = !connection_status;
        $("#online_status")[0].style.color = "green";
        $("#online_status")[0].innerText = "online";
        $("#status")[0].hidden = false;
    }
    var parsed_data = result.split( " ");
    switch(parsed_data[0]){
        case "tempo":
            tempo_ele.value = parseInt(parsed_data[1]);
            break;
        case "time":
            let time_val = parseInt(parsed_data[1]);
            document.getElementById("status_time").innerText = "device uptime:\n " + time_convert(time_val) + "  (H:M:S)";
            break;
    }
});

$("#tempoDecrease").click(()=>{
    if(connection_status){
        var val = parseInt(tempo_ele.value);
        val = val - 5 > 40 ?  val - 5 : 40;
        socket.emit('command',"tempo " + val + "\n");
    }
});

$("#tempoIncrease").click(()=>{
    if(connection_status){ 
        var val = parseInt(tempo_ele.value);
        val = val + 5 < 200 ?  val + 5 : 200;
        socket.emit('command',"tempo " + val + "\n");
    }
});

const time_convert = (seconds)=>{
    seconds = parseInt(seconds);
    let hour = seconds / 3600;
    seconds = seconds % 3600;
    let mins = seconds / 60;
    seconds = seconds % 60;
    return parseInt(hour) + " : " + parseInt(mins) + " : " + (seconds)
};

$( document ).ready(function() {
    window.setInterval(() =>{
        socket.emit('command',"uptime\n");
    },1000);
});

socket.emit('command',"tempo " + 120 + "\n");