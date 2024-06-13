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
        <button>Click me if you dare man</button>
      </header>
    </div>
  );
}

export default App;
