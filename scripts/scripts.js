// scripts.js
// this contains all the scripts used for the website

// openMenu is for redirecting to different pages
// scripts.js
function openMenu(event, menuName) {
    var i, tabcontent, tablinks;
    
    // Get all elements with class "sensor" and hide them
    tabcontent = document.getElementsByClassName("sensor");
    for (i = 0; i < tabcontent.length; i++) {
      tabcontent[i].style.display = "none";
    }
    
    // Get all elements with class "tablink" and remove the "active" class
    tablinks = document.getElementsByClassName("tablink");
    for (i = 0; i < tablinks.length; i++) {
      tablinks[i].classList.remove("active");
    }
    
    // Show the current tab, and add an "active" class to the button that opened the tab
    document.getElementById(menuName).style.display = "block";
    event.currentTarget.classList.add("active");
  }
  
  // Add an event listener to initialize the first tab when the page loads
  window.addEventListener("load", function() {
    document.querySelector(".tablink").click();
  });


// function to open and close menu
function toggleMenu(event, menuName) {
  var tabcontent = document.getElementById(menuName);
  var computedStyle = window.getComputedStyle(tabcontent);

  if (computedStyle.display === "block") {
    tabcontent.style.display = "none";
    event.currentTarget.classList.remove("active");
  } else {
    tabcontent.style.display = "block";
    event.currentTarget.classList.add("active");
  }
}

window.addEventListener("load", function () {
  document.querySelector(".tablink").click();
});

// Function to load and insert HTML content into a specified container
function loadHTMLContent(url, containerId) {
  const container = document.getElementById(containerId);

  fetch(url)
      .then(response => response.text())
      .then(data => {
          container.innerHTML = data;
      })
      .catch(error => {
          console.error("Error loading HTML content:", error);
      });
}

 
// smoothScrollTo is used to integrate smooth scrolling into the website
function smoothScrollTo(targetId) {
  const targetElement = document.getElementById(targetId);

  if (targetElement) {
    window.scrollTo({
      top: targetElement.offsetTop,
      behavior: 'smooth'
    });
  }
};

// Function for Animation with GSAP
function animateWithGSAP(elementId) {
    const elementToAnimate = document.getElementById(elementId);
  
    gsap.from(elementToAnimate, {
      opacity: 0,
      y: 50, // Move 50 pixels down
      duration: 1, // Animation duration in seconds
      ease: 'power2.inOut' // Easing function
    });
  }

// navigation toggle
function toggleNavigation() {
  const menuIcon = document.getElementById('menu-icon');
  const navMenu = document.getElementById('nav-menu');

  menuIcon.addEventListener('click', () => {
    navMenu.classList.toggle('open');
  });
}

// function to validate input
function validateInput(value) {
  if (isNaN(value)) {
      return "Please enter a valid numeric value.";
  }
  return null;  
}

// function to update status
function updateStatus(inputId, statusId, resultBoxId, conditionFunction) {
  var inputElement = document.getElementById(inputId);
  var statusElement = document.getElementById(statusId);
  var resultBoxElement = document.getElementById(resultBoxId);

  // Clear previous status, error messages, and styles
  statusElement.innerHTML = "";
  inputElement.style.border = "1px solid #ccc";
  resultBoxElement.innerHTML = ""; // Clear previous content
  resultBoxElement.style.backgroundColor = ""; // Clear previous background color

  var value = parseFloat(inputElement.value);
  var validationError = validateInput(value);

  if (validationError !== null) {
    statusElement.innerHTML = validationError;
    inputElement.style.border = "1px solid red";
    return;
  }

  // Perform condition-specific logic based on the provided condition function
  var result = conditionFunction(value);

  // Display the result in the resultBox
  resultBoxElement.innerHTML = result.text;

  // Update statusElement styles based on status
  if (result.text === "GOOD") {
    resultBoxElement.style.backgroundColor = "lightgreen"; // Set background color to green for "GOOD"
  } else if (result.text === "BAD") {
    resultBoxElement.style.backgroundColor = "mistyrose"; // Set background color to red for "BAD"
  } else {
    // Handle other status conditions and background colors here
    // You can add more conditions as needed
  }
}

function checkParameter(value){
  var selected = document.getElementById("parameterSelect").value;
  if(selected == "temperature")    return checkTemperature(value);
  if(selected == "pH")             return checkpH(value);
  if(selected == "humidity")       return checkhumidity(value);
  if(selected == "pressure")       return checkpressure(value);
}

// function for temperature
function checkTemperature(value) {
  if (value >=20 && value <=40) {
    if (value >= 28 && value <= 32) {
      return {
        text: "GOOD",
      };
    } else {
      return {
        text: "BAD",
      };
    }
  } else {
    return {
      text: "Out of acceptable temperature range (20-40)"
    };
  }
}

// function for pH
function checkpH(value) {
  if (value >= 0 && value <= 14) {
    if (value >= 5 && value <= 7) {
      return {
        text: "GOOD",
      };
    } else {
      return {
        text: "BAD",
      };
    }
  } else {
    return {
      text: "Out of acceptable pH range (0-14)",
    };
  }
}

// function for humidity
function checkhumidity(value) {
  if (value >= 0 && value <= 100) {
    if (value >= 20 && value <= 70) {
      return {
        text: "GOOD",
      };
    } else {
      return {
        text: "BAD",
      };
    }
  } else {
    return {
      text: "Out of acceptable moisture range (0%-100%)",
    };
  }
}

// function for pressure
function checkpressure(value) {
  if (value >= 0 && value <= 100) {
    if (value >= 4.1 && value <= 100) {
      return {
        text: "GOOD",
      };
    } else {
      return {
        text: "BAD",
      };
    }
  } else {
    return {
      text: "Out of acceptable pressure range (0-100KG/cm^2)",
    };
  }
}

// Function to open questions on FAQ page
function toggleAnswer(questionId) {
  var answer = document.getElementById(questionId);
  var allAnswers = document.querySelectorAll('.faq-container .faq-body');

  allAnswers.forEach(function (el) {
    if (el.id === questionId) {
      el.classList.toggle('open');
    } else {
      el.classList.remove('open');
    }
  });
}

let latestData = [];
let latestTemperature = null;
let latestPressure = null;

document.addEventListener("DOMContentLoaded", function () {
    fetchData();
    setInterval(fetchData, 10000); // Fetch data every 10 seconds (10000 milliseconds)
});

function fetchData() {
    // Make a GET request to your server to fetch the data
    fetch("http://localhost:3000/data")
        .then((response) => response.json())
        .then((data) => {
            //Sorting it from the latest data to the oldest data
            data.sort((a, b) => new Date(b.timestamp) - new Date(a.timestamp));

            // Filter and keep only the latest two entries
            latestData = data.filter((entry, index) => index < 2);

            // Display the data and infection status for the latest two entries
            displayData(latestData);

            // Find the temperature and pressure values in the JSON response
            const sensorData = data.find((entry) => entry.sensor_type === "temperature" || entry.sensor_type === "pressure");

            if (sensorData) {
                const temperature = sensorData.sensor_type === "temperature" ? sensorData.value : null;
                const pressure = sensorData.sensor_type === "pressure" ? sensorData.value : null;

                // Update the latest readings
                latestTemperature = temperature;
                latestPressure = pressure;

                // Define your threshold values
                const temperatureThreshold = 40; // Replace with your actual threshold
                const pressureThreshold = 4.1; // Replace with your actual threshold

                // Calculate infection risk level
                const infectionRisk = calculateInfectionRisk(latestTemperature, latestPressure, temperatureThreshold, pressureThreshold);

                // Display the data and infection status
                displayData(latestTemperature, latestPressure);
                displayInfectionStatus(infectionRisk);

                // Log the latestTemperature to the console
                console.log("Latest Temperature: " + latestTemperature);
            } else {
                console.error("No temperature or pressure data found in the JSON response.");
            }
        })
        .catch((error) => {
            console.error("Error fetching data:", error);
        });
}

function calculateInfectionRisk(temperature, pressure, temperatureThreshold, pressureThreshold) {
    if (temperature > temperatureThreshold && pressure > pressureThreshold) {
        return "High risk of wound development, please see the doctor";
    } else if (temperature > temperatureThreshold || pressure > pressureThreshold) {
        return "Moderate risk of wound development";
    } else {
        return "Low risk of wound development";
    }
}

function displayData(data) {
    for (let i = 0; i < data.length; i++) {
        const entry = data[i];
        const sensorType = entry.sensor_type;
        const value = entry.value;

        if (sensorType === "temperature") {
            document.getElementById("temperature" + (i + 1)).textContent = value + "°C";
        } else if (sensorType === "pressure") {
            document.getElementById("pressure" + (i + 1)).textContent = value + "N/cm²";
        }
    }
}

function displayInfectionStatus(infectionRisk) {
    document.getElementById("infection-status-text").textContent = "Infection Status: " + infectionRisk;
}
