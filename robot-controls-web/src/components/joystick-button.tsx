interface JoystickButtonProps {
  movement: string;
  setMovement: (movement: string) => void;
  children: React.ReactElement;
}
export default function JoystickButton({
  movement,
  setMovement,
  children,
}: JoystickButtonProps) {
  return (
    <button
      onMouseDown={() => setMovement(movement)}
      onTouchStart={() => setMovement(movement)}
      onMouseUp={() => setMovement("stop")}
      onTouchEnd={() => setMovement("stop")}
      className="hover:bg-yellow-500 p-2 rounded-full text-2xl text-secondary transition-colors"
    >
      {children}
    </button>
  );
}
