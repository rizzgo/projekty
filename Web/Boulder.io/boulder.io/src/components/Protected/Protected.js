/**
 * @author xseipe00
 */

import React, { useEffect, useState } from "react";
import { isLoggedIn } from "../Auth/Auth";

import Login from "../Login/Login";
import Register from "../Register/Register";

// pages available only to logged in user
export default function Protected({component}) {
    const [loginOpen, setLoginOpen] = useState(false);
    const [registerOpen, setRegisterOpen] = useState(false);
    const [output, setOutput] = useState(null);

    useEffect(()=>{
        if (isLoggedIn()) {
            setOutput(component)
        }
        else {
            setLoginOpen(true)
    
            setOutput(
                <React.Fragment>    
                <div>
                    <Login open={loginOpen} onClose={() => setLoginOpen(false)} onRegister={() => {setLoginOpen(false); setRegisterOpen(true)}}></Login>
                </div>
                <div>
                    <Register open={registerOpen} onClose={() => setRegisterOpen(false)} onRegister={() => {setRegisterOpen(false); setLoginOpen(true)}}></Register>
                </div>
                </React.Fragment>
            )
        }
    }, [component, loginOpen, registerOpen])

    return output
    
}
