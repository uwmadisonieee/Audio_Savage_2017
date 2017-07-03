// creates connection first
var ws = new WebSocket('ws://' + location.host);

// decides what do when message arrives
ws.onmessage = function(event) {

  var message = JSON.parse(event.data);
  console.log(message);    
  switch(message.type) {
    case "temperature":
      document.getElementById("degreeValue").innerHTML = message.value;
      drawNewTempBox(parseInt(message.value));
      break;
    default:
      console.log("No case for data");
      console.log(message);
  }

}

// sends back key when you press one
window.addEventListener('keydown', function(event) {
  ws.send("key:" + event.keyCode);
});

// sends song back when button pressed
function sendSong(file) {
  ws.send("song:" + file);
}

// Setup of canvas temp box
var tempBox = document.getElementById("tempBox");
var tempBoxCtx = tempBox.getContext("2d");
tempBoxCtx.fillStyle="#000000";
tempBoxCtx.fillRect(0,0, tempBox.width, tempBox.height);

function drawNewTempBox(newTemp) {   
  tempBoxCtx.fillStyle="#000000";
  tempBoxCtx.fillRect(0,0, tempBox.width, tempBox.height);

  var relativeHeight = (1 - (newTemp / 100)) * tempBox.height;
  tempBoxCtx.fillStyle="#FF0000";
  tempBoxCtx.fillRect(0,relativeHeight, tempBox.width, tempBox.height);
}