// MQTT Communication 
function startConnect(){
    const host = "167.235.232.87";
    // const host = "agiot.reparts.org";   
    const port = 9001;  
    const clientID = "0";
    
    document.getElementById("messages").innerHTML += "<span> Connecting to " + host + "on port " +port+"</span><br>";
    document.getElementById("messages").innerHTML += "<span> Using the client Id " + clientID +" </span><br>";

    client = new Paho.MQTT.Client(host, port, clientID);

    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = dummy;//onMessageArrived;

    client.connect({
        onSuccess: onConnect
    });
}

function onConnect(){
    // document.getElementById("messages").innerHTML += "<span> Subscribing to topic "+topic + "</span><br>";

    client.subscribe("gruppe2/pitch");
    client.subscribe("gruppe2/roll");
    client.subscribe("gruppe2/yaw");
}

function dummy(message) {
   console.log("OnMessageArrived: "+message.payloadString);
   document.getElementById("messages").innerHTML += "<span> Topic:"+message.destinationName+"| Message : "+message.payloadString + "</span><br>";

    var obj = JSON.parse(message.payloadString);
    
    document.getElementById("gyroX").innerHTML = obj.g[0];
    document.getElementById("gyroY").innerHTML = obj.g[1];
    document.getElementById("gyroZ").innerHTML = obj.g[2];

    // Change cube rotation after receiving the readinds
    cube.rotation.x = obj.g[1];
    cube.rotation.z = obj.g[0];
    cube.rotation.y = obj.g[2];
    renderer.render(scene, camera);

    document.getElementById("temp").innerHTML = obj.t;


    document.getElementById("accX").innerHTML = obj.a[0];
    document.getElementById("accY").innerHTML = obj.a[1];
    document.getElementById("accZ").innerHTML = obj.a[2];
}

function onConnectionLost(responseObject){
    document.getElementById("messages").innerHTML += "<span> ERROR: Connection is lost.</span><br>";
    if(responseObject !=0){
        document.getElementById("messages").innerHTML += "<span> ERROR:"+ responseObject.errorMessage +"</span><br>";
    }
}

function onMessageArrived(message){
    console.log("OnMessageArrived: "+message.payloadString);
    document.getElementById("messages").innerHTML += "<span> Topic:"+message.destinationName+"| Message : "+message.payloadString + "</span><br>";

    //console.log("gyro_readings", e.data);
    var obj = JSON.parse(message.payloadString);
    
    document.getElementById("gyroX").innerHTML = obj.g[0];
    document.getElementById("gyroY").innerHTML = obj.g[1];
    document.getElementById("gyroZ").innerHTML = obj.g[2];

    // Change cube rotation after receiving the readinds
    cube.rotation.x = obj.g[1];
    cube.rotation.z = obj.g[0];
    cube.rotation.y = obj.g[2];
    renderer.render(scene, camera);

    document.getElementById("temp").innerHTML = obj.t;


    document.getElementById("accX").innerHTML = obj.a[0];
    document.getElementById("accY").innerHTML = obj.a[1];
    document.getElementById("accZ").innerHTML = obj.a[2];
}

startConnect();




let scene, camera, rendered, cube;

function parentWidth(elem) {
  return elem.parentElement.clientWidth;
}

function parentHeight(elem) {
  return elem.parentElement.clientHeight;
}

function init3D(){
  scene = new THREE.Scene();
  scene.background = new THREE.Color(0xffffff);

  camera = new THREE.PerspectiveCamera(75, parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube")), 0.1, 1000);

  renderer = new THREE.WebGLRenderer({ antialias: true });
  renderer.setSize(parentWidth(document.getElementById("3Dcube")), parentHeight(document.getElementById("3Dcube")));

  document.getElementById('3Dcube').appendChild(renderer.domElement);

  // Create a geometry
  const geometry = new THREE.BoxGeometry(5, 1, 4);

  // Materials of each face
  var cubeMaterials = [
    new THREE.MeshBasicMaterial({color:0x03045e}),
    new THREE.MeshBasicMaterial({color:0x023e8a}),
    new THREE.MeshBasicMaterial({color:0x0077b6}),
    new THREE.MeshBasicMaterial({color:0x03045e}),
    new THREE.MeshBasicMaterial({color:0x023e8a}),
    new THREE.MeshBasicMaterial({color:0x0077b6}),
  ];

  const material = new THREE.MeshFaceMaterial(cubeMaterials);

  cube = new THREE.Mesh(geometry, material);
  scene.add(cube);
  camera.position.z = 5;
  renderer.render(scene, camera);
}

// Resize the 3D object when the browser window changes size
function onWindowResize(){
  camera.aspect = parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube"));
  //camera.aspect = window.innerWidth /  window.innerHeight;
  camera.updateProjectionMatrix();
  //renderer.setSize(window.innerWidth, window.innerHeight);
  renderer.setSize(parentWidth(document.getElementById("3Dcube")), parentHeight(document.getElementById("3Dcube")));

}

window.addEventListener('resize', onWindowResize, false);

// Create the 3D representation
init3D();

function resetPosition(element){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/"+element.id, true);
  console.log(element.id);
  xhr.send();
}
