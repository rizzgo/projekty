/**
 * @author xmikus18
 */
import React, {useState, useEffect }from 'react';
import logo from './pictures/profilePicture.png';
import './Profile.css'
import Edit from "./Edit"
import {IoLogOutOutline} from "react-icons/io5"
import {AiOutlineEdit} from "react-icons/ai";
import { getUsername, logOut, setUsername } from '../Auth/Auth';
import { useNavigate, useParams } from 'react-router';

function Profile(props) {

  // states
  const [editOpen, setEditOpen] = useState(false);
  const [profilePicture, setProfilePicture] = useState(logo)

  let rank = "Beginner";
  
  // load profile picture
  function convertFileToBase64(file) {
    return new Promise((resolve, reject) => {
        let base64String = "";
        let reader = new FileReader();   
    
        reader.onload = function () {
            base64String = reader.result.replace("data:", "").replace(/^.+,/, "");
            resolve(base64String)
        }
        reader.readAsDataURL(file);
    })
  }

  async function loadPicture(event){
      const picture = event.target.files[0]
      let image = await convertFileToBase64(picture)
      setProfile({...profile, "image": image})
  }

  const [profile, setProfile] =useState({
    username: "",
    image: "",
    description: ""
  })
  
  const navigate = useNavigate()

  useEffect(()=>{
    getUser()
  },[props])

  // get user data from server
  async function getUser() {
      try {
        const response = await fetch(`http://127.0.0.1:8000/users/${props.username}`, {})
        if (response.ok) {
          const data = await response.json()
          setProfile(data)
        }
      } catch(err) {
        console.error(err)
      }
  }

  return (
    <div className="wrapper">  
      <div className="user-grid-container">
        <div className="main">
        
          <div className="pictureItem">
            <img src={"data:image/png;base64," + profile.image} alt="Logo" className='profile'/> 
          </div>

          <div className="nameItem">
            <h2 className="nameText">{profile.username}</h2>
            <h3 className="rankText">{rank}</h3>
            <div>
              <input 
                id="file-input" className='profile-picture' type="file" 
                accept="image/*" onChange={loadPicture} style={{display: 'none',}}
              >
              </input>
              <label 
                className='editIconButton' for="file-input" 
                style={{display: editOpen ? '' : 'none',}}
              >
                Edit Picture
              </label>
            </div>
          </div>

          <div className="logout-button">
            <IoLogOutOutline onClick={() => {logOut(); navigate("/home")}}/>
          </div>

          <div className="editButton">
            <AiOutlineEdit 
              onClick={() => {setEditOpen(!editOpen)}} 
              style={{display: (!props.username || props.username == getUsername()) ? '' : 'none'}}
            />
          </div>

        </div>
        
        <div className="description" style={{display: !editOpen ? '' : 'none'}}>
          {console.log("profile", profile)}
          {profile.description}
        </div>
        

        <Edit onClose={setEditOpen} profile={profile} open={editOpen} type="text" desc={profile.description} setProfile={setProfile} ></Edit>
      
      </div>
    </div>
  );
}

export default Profile;
