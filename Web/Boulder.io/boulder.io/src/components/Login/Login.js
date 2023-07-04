/**
 * @author xbahni00
 */

import React, { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import ReactDom from "react-dom";
import { VscClose } from "react-icons/vsc";
import "./Login.css"
import { getUsername, setUsername } from "../Auth/Auth";



function Login({ open, onClose, onRegister }) {

    // state declarations
    const [details, setDetails] = useState({username: "", password: ""});
    const [error, setError] = useState(false);
    const navigate = useNavigate()

    // on open change check if login should be open
    useEffect(()=>{
        if (!open) return null;
    }, [open])

    // handling login form submit
    const handleSubmit = e => {
        e.preventDefault();
        authorize(details);
        navigate("/user");
    }

    // authorizing user on server
    async function authorize(details) {
        try {
            let data = JSON.stringify(details);
    
            let response = await fetch('http://127.0.0.1:8000/login', {
                method: "POST", 
                body: data, 
                mode: 'cors', 
                headers:{'Content-Type': 'application/json'}
            });
            
            if(response.status === 200) {
                setError(false);
                setUsername(details["username"])
                console.log(details["username"])
                navigate("/user")
                onClose();
            } else if( response.status === 401) {
                setError(true);
                setDetails({username:'', password:''})
            }
        } catch(error) {
            console.log(error);
        }
    }

    // error message template
    const errorMessage = () => {
        return (
            <div
              className="errorLogin"
              style={{
                display: error ? '' : 'none',
              }}>
              Incorrect username or password
              </div>
          );
    };

    // open registration window
    const handleRegSwitch = () => {
        setError(false)
        onRegister()
    }

    const closeOverlay = (e) => {
        if (e.target === e.currentTarget)
        {
            navigate("/home");
        }
    }

    return ReactDom.createPortal(

        <div className="loginOverlay" onClick={closeOverlay}>
                <form onSubmit={handleSubmit}>
                    <div className="loginForm">
                        <div>
                            <a><VscClose onClick={e => navigate("/home")} className="loginExit" /></a>
                        </div>
                        <div className="loginFormLabel">Username</div>

                        <div className="formGroup">
                            <label htmlFor="name"/>
                            <input className="loginFormInput" type="text" name="username" id="username" required onChange={e => setDetails({...details, username: e.target.value})} value={details.username}/>
                        </div>

                        <div className="loginFormLabel">Password</div>
                        <div className="formGroup">
                            <label htmlFor="password"/>
                            <input className="loginFormInput" type="password" name="password" id="password" required onChange={e => setDetails({...details, password: e.target.value})} value={details.password}/>
                        </div>
                        <button type="submit" className="loginFormSubmit">Log In</button>
                        <button onClick={handleRegSwitch} className="registerButton">Register</button>
                        <div>
                            {errorMessage()}
                        </div>
                    </div>
                </form>
        </div>
        , document.getElementById('portal')
    )
}

export default Login;