import React, { ReactNode } from 'react';
import './styles.scss';

interface Props {
  title: string;
  icon: ReactNode;
  onClick?(): void;
}

const Button = ({ title, icon, onClick }: Props) => {
  return (
    <button id="action-button" onClick={onClick}>
      {icon}
      <h2>{title}</h2>
    </button>
  );
};

export default Button;
