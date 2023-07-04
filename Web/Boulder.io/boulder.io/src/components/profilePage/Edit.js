/**
 * @author xbahni00
 */

import React, { useState, useImperativeHandle } from 'react'
import { getUsername } from '../Auth/Auth';

import "./Edit.css";


function Edit({ open, profile , onClose, desc, setProfile }) {

    if (!open) return null;

    // add changes to profile to server
    const handleSubmit = async () => {

        let user = {
            image: profile.image,
            description: document.getElementById("newDes").value, 
        }
        const requestOptions = {
            method: 'PUT',
            body: JSON.stringify(user),
            headers: {
                "Authorization": getUsername(), 
                "Content-type":"application/json" 
            },
            metod: "cors"
          };
        
        try{
            const res = await fetch("http://127.0.0.1:8000/users",requestOptions)
            if (res.ok) {
                const datas = await res.json()
                console.log("updated user", datas)
                setProfile(datas)
            }
        }
        catch (err){
            console.error(err)
        }

        console.log(document.getElementById("newDes").value)
        onClose()
    }
    
    return (
        <div className="editStyle">
            <textarea 
                type="text" 
                id="newDes"
                className="inputStyle"
                defaultValue={desc}
                rows="5"
                spellCheck="false">
            </textarea>
            <button type="submit" onClick={handleSubmit} className="submitEditChange">Submit</button>
        </div>
        
        
    )
}

export default Edit;
