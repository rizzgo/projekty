/**
 * @author xbahni00
 */

import React, { useState} from 'react'
import ReactDom from "react-dom";
import Switch from "react-switch";
import { VscClose } from "react-icons/vsc";
import { useNavigate } from "react-router-dom";
import { Link } from "react-router-dom";
import Slider from "./Slider";
import "./Filter.css";


/**
 * filter component body
 * @returns filter component
 */
function Filter({ open, onClose}) {
    // states declarations
    const [isSwitched, setSwitched] = useState(true);
    const [sliderState, setSliderState] = useState([0, 23]);
    const [values, setValues] = useState(["5A", "8C%2B"]);
    const [sortBy, setSortBy] = useState('Age');
    const navigate = useNavigate()

    // checking if filter should be open
    if (!open) return null;

    // handling filter switch data
    function handleSwitch() {
        console.log("is switched", isSwitched, "sort by", sortBy)

        if(isSwitched) {
            setSortBy('Rating')
        } else {
            setSortBy('Age')
        }
        setSwitched(!isSwitched)
    }

    function handleFilterButton() {
        onClose()
    }

    // updates values from filter
    function updateValues(values) {
        setValues(values)
    }

    const closeOverlay = (e) => {
        if (e.target === e.currentTarget)
        {
            onClose();
        }
    }
    
    return ReactDom.createPortal(

        <div className="filterOverlay" onClick={closeOverlay}>
            <div className="filterStyle">
                <div>
                    <div className="filterText">Choose dificulty range</div>
                    <a><VscClose className="filterExit" onClick={onClose}/></a>
                </div>
                <div style={{paddingTop: 10, paddingLeft: 20, paddingRight: 20, height: 60}}>
                    <Slider sliderState={sliderState} setSliderState={setSliderState} updateValues={updateValues}></Slider>
                </div>
                <div className="filterText">Sort by</div>
                <div className="filterText">
                    <div className="leftFloat">Rating</div>
                    <label style={{position: 'relative', left: -10}}>
                        <Switch 
                            width={100} className="react-switch-bg" background-color="white" offHandleColor="#3271d6" onHandleColor="#3271d6" 
                            boxShadow="2px 2px 2px 2px rgba(0, 0, 0, 0.1)" uncheckedIcon checkedIcon 
                            onColor="#e0e0e0" offColor="#e0e0e0" handleDiameter={35} onChange={handleSwitch} checked={isSwitched}>
                        </Switch>
                    </label>
                    <div className="rightFloat">Age</div>
                </div>
                <Link to={`?lowerGrade=${values[0]}&higherGrade=${values[1]}&sortType=${sortBy}`}><button className="filterFilterButton" onClick={handleFilterButton}>Filter</button></Link>
            </div>
        </div>
        , document.getElementById('portal')
    )
}

export default Filter;
