var mqtt = require('mqtt'); //https://www.npmjs.com/package/mqtt
var Topic = '#'; //subscribe to all topics
var Broker_URL = 'mqtt://192.168.100.3'; //Broker_URL
var Database_URL = 'localhost'; //Database_URL

var options = {
	clientId: 'MQTTNodeJsClientId',//MQTT Id of this for Broker
	port: 1883,	
	keepalive : 120
};
//Subscribed to all,clientId

var client  = mqtt.connect(Broker_URL, options);
client.on('connect', mqtt_connect);
client.on('reconnect', mqtt_reconnect);
client.on('error', mqtt_error);
client.on('message', mqtt_messsageReceived);
client.on('close', mqtt_close);

function mqtt_connect() {
    //console.log("Connecting MQTT");
    client.subscribe(Topic, mqtt_subscribe);
};

function mqtt_subscribe(err, granted) {
    console.log("Subscribed to " + Topic);
    if (err) {console.log(err);}
};

function mqtt_reconnect(err) {
    //console.log("Reconnect MQTT");
    //if (err) {console.log(err);}
	client  = mqtt.connect(Broker_URL, options);
};

function mqtt_error(err) {
    //console.log("Error!");
	//if (err) {console.log(err);}
};

function after_publish() {
	//do nothing
};

//receive a message from MQTT broker
function mqtt_messsageReceived(topic, message, packet) {
	console.log("\n topic: "+topic);
	console.log("\n message: "+message);
	
	//console.log("packet: "+packet);
	var message_str = message.toString(); //convert byte array to string
	message_str = message_str.replace(/\n$/, ''); //remove new line
	//payload syntax: clientID,message //Old Syntax
	if (countInstances(message_str) != 1) {
		console.log("Invalid payload"); //Incase payload must contain ClientID else use else Part directly
		parseString2JsonArray(topic,message_str,packet); // Message must in JSON Array Format
		//[{'client':'clientId'},message_str,{'value':'value'}]
		//my_packet i.e. Message: {'d':'dht','c':'','h':'"+hmdtChar+"','t':'"+tmprChar+"'}
	} 
	else {		
		insert_message(topic, message_str, packet);
		console.log("insert_message");
		//console.log(message_arr);
	}
};

function mqtt_close() {
	//console.log("Close MQTT");
};

///////////////////// MYSQL ////////////////////////

var mysql = require('mysql'); //https://www.npmjs.com/package/mysql
//Create Connection
var connection = mysql.createConnection({
	host: Database_URL,
	user: "root",
	password: "",
	database: "mosquito_mqtt"
});

connection.connect(function(err) {
	if (err) throw err;
	//console.log("Database Connected!");
});

//insert a row into the tbl_messages table
function insert_message(topic, message_str, packet) {
	var message_arr = extract_string(message_str); //split a string into an array
	var clientID= message_arr[0];
	var message = message_arr[1];
	var sql = "INSERT INTO ?? (??,??,??) VALUES (?,?,?)";
	var params = ['tbl_messages', 'clientID', 'topic', 'message', clientID, topic, message];
	sql = mysql.format(sql, params);	
	
	connection.query(sql, function (error, results) {
		if (error) throw error;
		console.log("Message added: " + message_str);
	}); 
};	

var delimiter = ",";

//split a string into an array of substrings
function extract_string(message_str) {
	var message_arr = message_str.split(delimiter); //convert to array	
	return message_arr;
};	

//count number of delimiters in a string
function countInstances(message_str) {
	var substrings = message_str.split(delimiter);
	return substrings.length - 1;
};

function parseString2JsonArray(topic,string_data,packet){
	var obj = JSON.parse(string_data.replace(/'/g, '"'));
	console.log(string_data.replace(/'/g, '"'));

	if(obj.length<3){
		console.log("Invalid Message From Client");	
	}else{
		console.log(obj.length);
		
		var message_str = JSON.stringify(obj[1]);//Message fetch from message_str(JSON String)
		
		console.log("client: "+obj[0].client);//client fetch from message_str(JSON String)
		console.log("Message: "+message_str);
		console.log("Value: "+obj[2].value);//value(i.e. Message No) fetch from message_str(JSON String)

		console.log("insert_message");
		
		insert_json_string_message(topic, obj[0].client,message_str, packet);
	}
};

//insert a row into the tbl_messages table
function insert_json_string_message(topic, client_id,message_str, packet) {
	
	var sql = "INSERT INTO ?? (??,??,??) VALUES (?,?,?)";
	var params = ['tbl_messages', 'clientID', 'topic', 'message', client_id, topic, message_str];
	sql = mysql.format(sql, params);	
	
	connection.query(sql, function (error, results) {
		if (error) throw error;
		console.log("Message added: " + message_str);
	}); 
};