import React, { Component } from 'react'
import { Slider, Rail, Handles, Tracks, Ticks } from 'react-compound-slider'
import { SliderRail, Handle, Track, Tick } from './SliderComponents'

const sliderStyle = {
  position: 'relative',
  width: '100%',
}

const defaultValues = [0, 23]

const gradeMapper = {
  0: "5A",
  1: "5A+",
  2: "5B",
  3: "5B+",
  4: "5C",
  5: "5C+",
  6: "6A",
  7: "6A+",
  8: "6B",
  9: "6B+",
  10: "6C",
  11: "6C+",
  12: "7A",
  13: "7A+",
  14: "7B",
  15: "7B+",
  16: "7C",
  17: "7C+",
  18: "8A",
  19: "8A+",
  20: "8B",
  21: "8B+",
  22: "8C",
  23: "8C+"
}

// slider component base code
class Example extends Component {

    state = {
      domain: [0,23],
      values: this.props.sliderState.slice(),
      update: this.props.sliderState.slice(),
      reversed: false,
    }
  
  onUpdate = update => {
    this.setState({ update })
  }
  
  onChange = values => {
    this.setState({ values })
    let lowerGrade = encodeURIComponent(gradeMapper[values[0]])
    let higherGrade = encodeURIComponent(gradeMapper[values[1]]) 
    console.log([lowerGrade, higherGrade])
    this.props.updateValues([lowerGrade, higherGrade]);
    this.props.setSliderState(values)
  }
  
  setDomain = domain => {
    this.setState({ domain })
  }
  
  toggleReverse = () => {
    this.setState(prev => ({ reversed: !prev.reversed }))
  }
  
  render() {
    const {
      state: { 
        domain, 
        values, 
        reversed },
    } = this

    return (
      <div style={{ height: 50, width: '100%', top: 40}}>
        <Slider
          mode={1}
          step={1}
          domain={domain}
          reversed={reversed}
          rootStyle={sliderStyle}
          onUpdate={this.onUpdate}
          onChange={this.onChange}
          values={values}
        >
          <Rail>
            {({ getRailProps }) => <SliderRail getRailProps={getRailProps} />}
          </Rail>
          <Handles>
            {({ handles, getHandleProps }) => (
              <div className="slider-handles">
                {handles.map(handle => (
                  <Handle
                    key={handle.id}
                    handle={handle}
                    domain={domain}
                    getHandleProps={getHandleProps}
                  />
                ))}
              </div>
            )}
          </Handles>
          <Tracks left={false} right={false}>
            {({ tracks, getTrackProps }) => (
              <div className="slider-tracks">
                {tracks.map(({ id, source, target }) => (
                  <Track
                    key={id}
                    source={source}
                    target={target}
                    getTrackProps={getTrackProps}
                  />
                ))}
              </div>
            )}
          </Tracks>
          <Ticks count={23}>
            {({ ticks }) => (
              <div className="slider-ticks">
                {ticks.map(tick => (
                  <Tick key={tick.id} tick={tick} count={ticks.length} />
                ))
                }
              </div>
            )}
          </Ticks>
        </Slider>
      </div>
    )
  }
}

export default Example