const express = require('express');
const https = require('https');
const fs = require('fs');
const sqlite3 = require('sqlite3').verbose();
const cors = require('cors');
const nodemailer = require('nodemailer');

const app = express();
const port = 3000; // Choose a port of your preference

// Middleware to enable CORS
app.use(cors());

// Middleware to parse JSON requests
app.use(express.json());

// Create or open the SQLite database
const db = new sqlite3.Database('mydatabase.db', (err) => {
  if (err) {
    console.error('Error opening database:', err.message);
  } else {
    console.log('Database opened successfully');
  }
});

// Define a route to handle POST requests
app.post('/data', (req, res) => {
  const data = req.body; // Received data from the Arduino
  console.log('Received data:', data); // Log the received data for debugging
  const sensor_type = data.sensor_type; // Extract the sensor type from the request

  if (sensor_type === 'temperature' || sensor_type === 'pressure') {
    // Insert the data into the database
    const timestamp = new Date().toISOString(); // Add a timestamp
    db.run(
      'INSERT INTO data (timestamp, sensor_type, value) VALUES (?, ?, ?)',
      [timestamp, sensor_type, data.value],
      (err) => {
        if (err) {
          console.error('Error inserting data into the database:', err.message);
          res.status(500).json({ error: 'Internal Server Error' });
        } else {
          console.log('Data inserted into the database');
          res.status(200).json({ message: 'Data received and stored successfully' });

          // Check if the threshold is crossed and send an email
          if (thresholdCrossed(data)) {
            sendEmailNotification();
          }
        }
      }
    );
  } else {
    res.status(400).json({ error: 'Invalid sensor_type' });
  }
});

// Function to send an email notification
function sendEmailNotification() {
  // Create a transporter object using the default SMTP transport
  const transporter = nodemailer.createTransport({
    host: 'smtp.gmail.com', // Replace with your SMTP server hostname
    port: 587, // Replace with the appropriate port number
    secure: false, // Set to true if your SMTP server uses SSL
    auth: {
      user: 'mddsmartbandage008@gmail.com', // Your email address
      pass: 'CCEBSTUDENT_001', // Your email password
    },
  });

  // Define the email data
  const mailOptions = {
    from: 'mddsmartbandage008@gmail.com', // Sender's email address
    to: 'echan018@e.ntu.edu.sg', // Recipient's email address
    subject: 'High Infection Risk Alert',
    text: 'The threshold value has been crossed.', // Email content
  };

  // Send the email
  transporter.sendMail(mailOptions, (error, info) => {
    if (error) {
      console.error('Error sending email:', error);
    } else {
      console.log('Email sent:', info.response);
    }
  });
}

// Define the threshold condition based on the data received from the client
function thresholdCrossed(data) {
  // Defining threshold for temperature and pressure
  const temperatureThreshold = 38.0;
  const pressureThreshold = 4.1;

  // Check if the data received from the client crosses the threshold
  if (data.sensor_type === 'temperature' && data.value > temperatureThreshold) {
    return true;
  }
  if (data.sensor_type === 'pressure' && data.value > pressureThreshold) {
    return true;
  }
  return false;
}

// Define a route to retrieve data from the 'data' table
app.get('/data', (req, res) => {
  db.all('SELECT * FROM data', (err, rows) => {
    if (err) {
      console.error('Error retrieving data from the database:', err.message);
      res.status(500).json({ error: 'Internal Server Error' });
    } else {
      console.log('Data retrieved from the database');
      res.status(200).json(rows);
    }
  });
});
// Specify the paths to your SSL/TLS certificate and private key
const privateKeyPath = '/Users/ervinchan/Documents/MDDProject/private-key.pem';
const certificatePath = '/Users/ervinchan/Documents/MDDProject/certificate.pem';
const caPath = '/Users/ervinchan/Documents/MDDProject/ca.pem';

// Read the SSL/TLS certificate and private key
const privateKey = fs.readFileSync(privateKeyPath, 'utf8');
const certificate = fs.readFileSync(certificatePath, 'utf8');
const ca = fs.readFileSync(caPath, 'utf8');

const credentials = { key: privateKey, cert: certificate, ca: ca };

// Create an HTTPS server using the credentials
const httpsServer = https.createServer(credentials, app);

// Start the server on the specified port
httpsServer.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});

// Handle server shutdown
process.on('SIGINT', () => {
  console.log('Closing database and server...');
  db.close((err) => {
    if (err) {
      console.error('Error closing database:', err.message);
    } else {
      console.log('Database closed.');
      process.exit(0); // Terminate the server process
    }
  });
});
