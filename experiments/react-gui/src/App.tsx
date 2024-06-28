import React from 'react';
import logo from './logo.svg';
import './App.css';

function App() {
  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <p>
          Click on the button to change pattern.
        </p>
        <button onClick={toggleLED}>Click me if you dare man</button>
      </header>
    </div>
  );
}

function toggleLED(){
  fetch('/toggle')
    .then(response => console.log(response.text))
}

export default App;
