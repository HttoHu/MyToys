const net = require('net');

let client = net.Socket();

client.connect({
    port: 6832,
    host: "localhost"
}, () => {
    console.log("connected to server!");
    const msg = JSON.stringify({
        req: "message-send",
        no: 1,
        auth: {
            username: "Htto",
            password: '12345'
        },
        content: {
            "data": "hello world"
        },
        dest: "Htto"
    });

    let buf = Buffer.alloc(4 + msg.length);
    buf.writeUInt32BE(msg.length);
    buf.write(msg, 4);
    client.write(buf);
})

client.on('data', (data) => {
    console.log(data.toString());
})

client.on('close', (data) => {
    console.log(data);
})