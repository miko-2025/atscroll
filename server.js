const ws = require('ws');
const http = require("http");
const reader = require("reader");

reader.set("sfile.txt");
const size = reader.getFileSize();

async function client(){
	const socket = new WebSocket('ws://localhost:5080');
	const body = document.body;
	const out = document.querySelector(".out");
	const ctrl = document.querySelector(".ctrl");

	body.style.padding = 0;
	body.style.margin = 0;
	body.style.height = "100px";

	const max = 5000;

	socket.onopen = () => {
		console.log('Connected!');
		socket.send(`0:${max}`);
	};

	socket.onmessage = (event) => {
		const [ size, ...content ] = event.data
			.toString()
			.split(':')
		;

		ctrl.style.height = `${size}px`;
		ctrl.style.width = `100vw`;
		/*ctrl.style.border = "2px solid red";
		ctrl.style.zIndex = "9";
		ctrl.style.backgroundColor = "rgba(255, 0, 0, 0.2)";
		out.style.border = "2px solid green";
		out.style.pointerEvents = "none";*/
		out.textContent = content.join();
	};

	socket.onerror = (error) => {
		console.error('WebSocket Error:', error);
	};

	document.scrollingElement.onscroll = function(){
		//out.textContent = window.scrollY;
		socket.send(`${window.scrollY}:${max}`);
	}
}

http.createServer(function(req, res){
	const html = (`


<html>
	<head>
		<script src="http://localhost:8080/Defensk/rote/moonsole.js">
		</script>
		<script>
			${client}
		</script>
	</head>
	<body style="white-space: pre; height: 100%; overflow-y: scroll">
		<div class="out" style="position: fixed; top: 0px"></div>
		<div class="ctrl" style="position: relative; top: 0px"></div>
		<script>
			client();
		</script>
	</body>
</html>


	`);

	//console.log(html);
	res.end(html);
}).listen({ port: 5081 });

const wss = new ws.WebSocketServer({ port: 5080 });
wss.on('connection', function connection(ws) {
	console.log('Client connected');

	ws.on('message', function incoming(message) {
		//console.log(`Received: ${message}`);
		// Echo the message back to the client
		//ws.send(`Server received: ${message}`);
		const [ pos, limit ] = message.toString()
			.split(':')
			.map(arg => ~~Number(arg))
		;
		//console.log(`${pos}:${limit}`);
		ws.send(`${size}:${reader.get(pos, limit)}`);
	});

	ws.on('close', function close() {
		console.log('Client disconnected');
	});
});

/*
let i = 0;
setInterval(function(){
	if(i >= size){
		i = 0;
		return ;
	}

	console.clear();
	console.log(reader.get(i, 1000));
	i++;
}, 10);
*/
