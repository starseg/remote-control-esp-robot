"use client";
import {
  ArrowBigDown,
  ArrowBigLeft,
  ArrowBigRight,
  ArrowBigUp,
  X,
} from "lucide-react";
import { Slider } from "@/components/ui/slider";
import { useEffect, useState } from "react";
import { client } from "@/lib/mqtt";
import JoystickButton from "@/components/joystick-button";

export default function Home() {
  const [movement, setMovement] = useState("stop");
  const [speed, setSpeed] = useState([20]);

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
          <div className="bg-stone-800 rounded-xl w-72 aspect-video">
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
