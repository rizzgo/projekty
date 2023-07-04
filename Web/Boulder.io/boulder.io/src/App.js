/**
 * 
 * @author xseipe00
 * 
 */

import React from "react"
import {BrowserRouter as Router, Routes, Route} from "react-router-dom"

import './App.css';
import Layout from "./components/Layout/Layout";
import NotFound from "./components/NotFound/NotFound";
import User from "./components/User/User";
import NewPost from "./components/NewPost/NewPost";
import Home from "./components/Home/Home";
import Protected from "./components/Protected/Protected";

/**
 * 
 * @returns main function body
 */
function App() {
  return (
    <React.Fragment>
    <Router>
      <Routes>
        <Route path="" element={<Layout/>}>
          <Route path="home/*" element={<Home/>}/>
          <Route path="new" element={<Protected component={<NewPost/>}/>}/>
          <Route path="user" element={<Protected component={<User/>}/>}/>
          <Route path="user/:username" element={<User/>}/>
        </Route>
        <Route path="*" element={<NotFound/>}/>
      </Routes>
    </Router>
    </React.Fragment>
  );
}

export default App;
