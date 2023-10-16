const express = require('express');
const sqlite3 = require('sqlite3').verbose();

const app = express();
const port = 3000; // Choose a port of your preference

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
  const sensor_type = data.sensor_type; // Extract the sensor type from the request

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
      }
    }
  );
});

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

// Start the server
app.listen(port, () => {
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
