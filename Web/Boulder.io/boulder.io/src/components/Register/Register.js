/**
 * @author xbahni00
 */
import React, { useState } from "react";
import ReactDom from "react-dom";
import { VscClose } from "react-icons/vsc";
import "./Register.css"
import { useNavigate } from "react-router-dom";
import {image} from "./defaultPicture"

function Register({ open, onClose, onRegister }) {

    // states
    const [name, setName] = useState('');
    const [mail, setMail] = useState('');
    const [pwd, setPwd] = useState('');
    const [confPwd, setConfPwd] = useState('');

    const [submitted, setSubmitted] = useState(false);
    const [error, setError] = useState(false);

    
    
    const navigate = useNavigate()

    if (!open) return null;

    const handleName = (e) => {
        setName(e.target.value);
        setSubmitted(false);
    }

    const handleMail = (e) => {
        setMail(e.target.value);
        setSubmitted(false);
    }

    const handlePwd = (e) => {
        setPwd(e.target.value);
        setSubmitted(false);
    }

    const handleConfPwd = (e) => {
        setConfPwd(e.target.value);
        setSubmitted(false);
    }

    // on submit check form legitimity and register
    const handleSubmit = e => {
        e.preventDefault();

        console.log(pwd);
        console.log(confPwd);

        if(pwd !== confPwd || name === '' || mail === '' || pwd === '' || confPwd === '') {
            setError(true);
            setPwd('');
            setConfPwd('');
        } else {
            register();
        }
    }

    const closeOverlay = (e) => {
        if (e.target === e.currentTarget)
        {
            navigate("/home");
        }
    }

    // add user registration to server
    async function register() {
        var data = JSON.stringify({
            image: image,
            username: name,
            password: pwd,
            email: mail
        });

        console.log(data);

        let response = await fetch('http://127.0.0.1:8000/users', {
                method: "POST", 
                body: data, 
                mode: 'cors', 
                headers:{'Content-Type': 'application/json'}
        });

        if(response.status === 200) {
            setError(false);
            onRegister();
            resetData();
        } else if( response.status === 409) {
            setError(true);
            setName('');
            setMail('');
        }
    }

    // reset form data
    const resetData = () => {
        setName('');
        setMail('');
        setPwd('');
        setConfPwd('');
        setError(false)
        setSubmitted(false);
        onClose();
    }

    // template for success and error messages
    const successMessage = () => {
        return (
            <div
            className="success"
            style={{
                display: submitted ? '' : 'none',
            }}>
            Registration succesful
            </div>
        );
    };

    const errorMessage = () => {
        return (
          <div
            className="error"
            style={{
              display: error ? '' : 'none',
            }}>
            Registration error
          </div>
        );
      };

    return ReactDom.createPortal(

        <div className="registerOverlay" onClick={closeOverlay}>
            <div className="registerStyle">
                <form name="regForm"onSubmit={handleSubmit}>
                    <div className="registerForm">
                        <div className="registerFormLabel">Username</div>
                        <div className="formGroup">
                            <label htmlFor="name"/>
                            <input className="registerFormInput" type="text" name="name" id="name" required onChange={handleName} value={name}/>
                        </div>


                        <div className="registerFormLabel">E-mail</div>
                        <div className="formGroup">
                            <label htmlFor="mail"/>
                            <input className="registerFormInput" type="email" name="mail" id="mail" required onChange={handleMail} value={mail}/>
                        </div>

                        <div className="registerFormLabel">Password</div>
                        <div className="formGroup">
                            <label htmlFor="password"/>
                            <input className="registerFormInput" type="password" name="password" id="password" required onChange={handlePwd} value={pwd}/>
                        </div>

                        <div className="registerFormLabel">Confirm password</div>
                        <div className="formGroup">
                            <label htmlFor="password"/>
                            <input className="registerFormInput" type="password" name="confPassword" id="confPassword" required onChange={handleConfPwd} value={confPwd}/>
                        </div>

                        <button type="submit" className="registerFormSubmit" >Register</button>
                        <div className="messages">
                            {errorMessage()}
                            {successMessage()}
                        </div>
                        <button className="registerExit" onClick={resetData}><VscClose></VscClose></button>
                    </div>
                </form>
            </div>
        </div>
        , document.getElementById('portal')
    )
}

export default Register;