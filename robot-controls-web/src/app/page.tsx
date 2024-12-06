"use client";
import {
  ArrowBigDown,
  ArrowBigLeft,
  ArrowBigRight,
  ArrowBigUp,
  BellRing,
  Lightbulb,
  X,
} from "lucide-react";
import { Slider } from "@/components/ui/slider";
import { useEffect, useState } from "react";
import { client } from "@/lib/mqtt";
import JoystickButton from "@/components/joystick-button";
import { Button } from "@/components/ui/button";

export default function Home() {
  const [movement, setMovement] = useState("stop");
  const [speed, setSpeed] = useState([20]);
  const [led, setLed] = useState(false);
  const [honk, setHonk] = useState(false);

  useEffect(() => {
    client.publish("robot/move", movement);
  }, [movement]);

  useEffect(() => {
    client.publish("robot/speed", speed.toString());
  }, [speed]);

  // useEffect(() => {
  //   client.on("message", (topic, message) => {
  //     if (topic === "robot/speed") {
  //       setSpeed([parseInt(message.toString())]);
  //     }
  //   });
  // })

  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      switch (e.key) {
        case "ArrowUp":
          setMovement("forward");
          break;
        case "ArrowDown":
          setMovement("backward");
          break;
        case "ArrowLeft":
          setMovement("left");
          break;
        case "ArrowRight":
          setMovement("right");
          break;
      }
    };

    const handleKeyUp = (e: KeyboardEvent) => {
      // Quando a tecla é solta, o robô deve parar
      setMovement("stop");
    };

    window.addEventListener("keydown", handleKeyDown);
    window.addEventListener("keyup", handleKeyUp);

    return () => {
      window.removeEventListener("keydown", handleKeyDown);
      window.removeEventListener("keyup", handleKeyUp);
    };
  }, []);

  function changeLed() {
    setLed(!led);
    client.publish("robot/led", led.toString());
  }

  function handleHonk(state: boolean) {
    setHonk(state);
    client.publish("robot/honk", state.toString());
  }

  return (
    <div className="bg-black mx-auto mt-4 p-2 border rounded-xl max-w-screen-sm">
      <h1 className="font-black text-4xl text-center text-primary">Star Seg</h1>
      <div className="flex justify-between items-center my-2">
        <div className="flex flex-col justify-between items-center bg-primary mx-4 rounded-full w-60 h-60">
          <JoystickButton movement="forward" setMovement={setMovement}>
            <ArrowBigUp size={56} />
          </JoystickButton>
          <div className="flex justify-between items-center w-full">
            <JoystickButton movement="left" setMovement={setMovement}>
              <ArrowBigLeft size={56} />
            </JoystickButton>
            <X size={56} className="text-secondary" />
            <JoystickButton movement="right" setMovement={setMovement}>
              <ArrowBigRight size={56} />
            </JoystickButton>
          </div>
          <JoystickButton movement="backward" setMovement={setMovement}>
            <ArrowBigDown size={56} />
          </JoystickButton>
        </div>
        <div className="flex flex-col gap-2 mx-4">
          <div className="bg-stone-800 p-2 rounded-xl w-72 aspect-video">
            <Button variant={led ? "default" : "outline"} onClick={changeLed}>
              <Lightbulb />
            </Button>
            <Button
              variant={honk ? "default" : "outline"}
              onMouseDown={() => handleHonk(true)}
              onMouseUp={() => handleHonk(false)}
            >
              <BellRing />
            </Button>
            {/* {video && <img src={video} alt="Streaming do robô" />} */}
            {/* <video ref={videoRef} controls autoPlay className="w-full" /> */}
          </div>
          <p className="mt-2 text-lg">Velocidade: {speed}</p>
          <Slider max={100} step={10} value={speed} onValueChange={setSpeed} />
        </div>
      </div>
    </div>
  );
}
