import React from 'react';
import './styles.scss';

import Switch from 'react-switch';

interface Props {
  checked: boolean;
  onChange(): void;
}

const SwitchButton = ({ checked, onChange }: Props) => {
  return (
    <Switch
      checked={checked}
      onChange={onChange}
      checkedIcon={false}
      uncheckedIcon={false}
      height={10}
      width={40}
      handleDiameter={20}
      onHandleColor="#cfcfcf"
      offHandleColor="#cfcfcf"
      onColor="#3A3AE7"
      offColor="#575757"
    />
  );
};

export default SwitchButton;
