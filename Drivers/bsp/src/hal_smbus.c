/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#include "hal_smbus.h"

#define SMBUS_TIMEOUT_BUSY_FLAG     25U         /*!< Timeout 25 ms             */
#define SMBUS_NO_OPTION_FRAME       0xFFFF0000U /*!< XferOptions default value */

#define SMBUS_STATE_NONE            ((uint32_t)(HAL_I2C_MODE_NONE))                                                        /*!< Default Value                                          */
#define SMBUS_STATE_MSK             ((uint32_t)((uint32_t)((uint32_t)HAL_I2C_STATE_BUSY_TX | (uint32_t)HAL_I2C_STATE_BUSY_RX) & (uint32_t)(~((uint32_t)HAL_I2C_STATE_READY)))) /*!< Mask State define, keep only RX and TX bits            */
#define SMBUS_STATE_MASTER_BUSY_TX  ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_TX & SMBUS_STATE_MSK) | (uint32_t)HAL_I2C_MODE_MASTER))            /*!< Master Busy TX, combinaison of State LSB and Mode enum */
#define SMBUS_STATE_MASTER_BUSY_RX  ((uint32_t)(((uint32_t)HAL_I2C_STATE_BUSY_RX & SMBUS_STATE_MSK) | (uint32_t)HAL_I2C_MODE_MASTER))            /*!< Master Busy RX, combinaison of State LSB and Mode enum */

static HAL_StatusTypeDef SMBUS_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef SMBUS_MasterRequestWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef SMBUS_MasterRequestRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef SMBUS_WaitOnTXEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef SMBUS_WaitOnBTFFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef SMBUS_WaitOnRXNEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef SMBUS_WaitOnMasterAddressFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, uint32_t Timeout, uint32_t Tickstart);
static HAL_StatusTypeDef SMBUS_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c);

/**
  * @brief  Transmits and Receive in master mode an amount of data in blocking mode.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  command Target command buffer
  * @param  rxData Pointer to data buffer
  * @param  rxSize Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2C_SMbus_Data(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *command, uint8_t *rxData, uint16_t rxSize, uint32_t Timeout)
{
  /* Init tickstart for timeout management*/
  uint32_t tickstart = HAL_GetTick();

  if (hi2c->State == HAL_I2C_STATE_READY)
  {
    /* Wait until BUSY flag is reset */
    if (SMBUS_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BUSY, SET, SMBUS_TIMEOUT_BUSY_FLAG, tickstart) != HAL_OK)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hi2c);

    /* Check if the I2C is already enabled */
    if ((hi2c->Instance->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
    {
      /* Enable I2C peripheral */
      __HAL_I2C_ENABLE(hi2c);
    }

    /* Disable Pos */
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);

    hi2c->State       = HAL_I2C_STATE_BUSY_TX;
    hi2c->Mode        = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = command;
    hi2c->XferCount   = 1U;
    hi2c->XferSize    = hi2c->XferCount;
    hi2c->XferOptions = SMBUS_NO_OPTION_FRAME;

    /* Send Slave Address */
    if (SMBUS_MasterRequestWrite(hi2c, DevAddress, Timeout, tickstart) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    if (SMBUS_WaitOnTXEFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
    {
      if (hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Generate Stop */
        SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
      }
      return HAL_ERROR;
    }

    /* Write data to DR */
    hi2c->Instance->DR = *hi2c->pBuffPtr;

    /* Wait until BTF flag is set */
    if (SMBUS_WaitOnBTFFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
    {
      if (hi2c->ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Generate Stop */
        SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
      }
      return HAL_ERROR;
    }

    hi2c->State       = HAL_I2C_STATE_BUSY_RX;
    hi2c->Mode        = HAL_I2C_MODE_MASTER;
    hi2c->ErrorCode   = HAL_I2C_ERROR_NONE;

    /* Prepare transfer parameters */
    hi2c->pBuffPtr    = rxData;
    hi2c->XferCount   = rxSize;
    hi2c->XferSize    = hi2c->XferCount;
    hi2c->XferOptions = SMBUS_NO_OPTION_FRAME;
    /* Send Slave Address */
    if (SMBUS_MasterRequestRead(hi2c, DevAddress, Timeout, tickstart) != HAL_OK)
    {
      return HAL_ERROR;
    }

    if (hi2c->XferSize == 0U)
    {
      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

      /* Generate Stop */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
    }
    else if (hi2c->XferSize == 1U)
    {
      /* Disable Acknowledge */
      CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

      /* Generate Stop */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);
    }
    else if (hi2c->XferSize == 2U)
    {
      /* Disable Acknowledge */
      CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

      /* Enable Pos */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_POS);

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }
    else
    {
      /* Enable Acknowledge */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(hi2c);
    }

    while (hi2c->XferSize > 0U)
    {
      if (hi2c->XferSize <= 3U)
      {
        /* One byte */
        if (hi2c->XferSize == 1U)
        {
          /* Wait until RXNE flag is set */
          if (SMBUS_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
          {
            return HAL_ERROR;
          }

          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;
        }
        /* Two bytes */
        else if (hi2c->XferSize == 2U)
        {
          /* Wait until BTF flag is set */
          if (SMBUS_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout, tickstart) != HAL_OK)
          {
            return HAL_ERROR;
          }

          /* Generate Stop */
          SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;

          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;
        }
        /* 3 Last bytes */
        else
        {
          /* Wait until BTF flag is set */
          if (SMBUS_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout, tickstart) != HAL_OK)
          {
            return HAL_ERROR;
          }

          /* Disable Acknowledge */
          CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;

          /* Wait until BTF flag is set */
          if (SMBUS_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_BTF, RESET, Timeout, tickstart) != HAL_OK)
          {
            return HAL_ERROR;
          }

          /* Generate Stop */
          SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;

          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;
        }
      }
      else
      {
        /* Wait until RXNE flag is set */
        if (SMBUS_WaitOnRXNEFlagUntilTimeout(hi2c, Timeout, tickstart) != HAL_OK)
        {
          return HAL_ERROR;
        }

        /* Read data from DR */
        *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

        /* Increment Buffer pointer */
        hi2c->pBuffPtr++;

        /* Update counter */
        hi2c->XferSize--;
        hi2c->XferCount--;

        if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == SET)
        {
          /* Read data from DR */
          *hi2c->pBuffPtr = (uint8_t)hi2c->Instance->DR;

          /* Increment Buffer pointer */
          hi2c->pBuffPtr++;

          /* Update counter */
          hi2c->XferSize--;
          hi2c->XferCount--;
        }
      }
    }


    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->Mode = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  This function handles I2C Communication Timeout.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  Flag specifies the I2C flag to check.
  * @param  Status The new Flag status (SET or RESET).
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef SMBUS_WaitOnFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, FlagStatus Status, uint32_t Timeout, uint32_t Tickstart)
{
  /* Wait until flag is set */
  while (__HAL_I2C_GET_FLAG(hi2c, Flag) == Status)
  {
    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {
        hi2c->PreviousState     = SMBUS_STATE_NONE;
        hi2c->State             = HAL_I2C_STATE_READY;
        hi2c->Mode              = HAL_I2C_MODE_NONE;
        hi2c->ErrorCode         |= HAL_I2C_ERROR_TIMEOUT;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_ERROR;
      }
    }
  }
  return HAL_OK;
}

/**
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef SMBUS_MasterRequestWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout, uint32_t Tickstart)
{
  /* Declaration of temporary variable to prevent undefined behavior of volatile usage */
  uint32_t CurrentXferOptions = hi2c->XferOptions;

  /* Generate Start condition if first transfer */
  if ((CurrentXferOptions == I2C_FIRST_AND_LAST_FRAME) || (CurrentXferOptions == I2C_FIRST_FRAME) || (CurrentXferOptions == SMBUS_NO_OPTION_FRAME))
  {
    /* Generate Start */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);
  }
  else if (hi2c->PreviousState == SMBUS_STATE_MASTER_BUSY_RX)
  {
    /* Generate ReStart */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);
  }
  else
  {
    /* Do nothing */
  }

  /* Wait until SB flag is set */
  if (SMBUS_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
  {
    /* Send slave address */
    hi2c->Instance->DR = I2C_7BIT_ADD_WRITE(DevAddress);
  }
  else
  {
    /* Send header of slave address */
    hi2c->Instance->DR = I2C_10BIT_HEADER_WRITE(DevAddress);

    /* Wait until ADD10 flag is set */
    if (SMBUS_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADD10, Timeout, Tickstart) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Send slave address */
    hi2c->Instance->DR = I2C_10BIT_ADDRESS(DevAddress);
  }

  /* Wait until ADDR flag is set */
  if (SMBUS_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout, Tickstart) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Master sends target device address for read request.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef SMBUS_MasterRequestRead(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout, uint32_t Tickstart)
{
  /* Declaration of temporary variable to prevent undefined behavior of volatile usage */
  uint32_t CurrentXferOptions = hi2c->XferOptions;

  /* Enable Acknowledge */
  SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

  /* Generate Start condition if first transfer */
  if ((CurrentXferOptions == I2C_FIRST_AND_LAST_FRAME) || (CurrentXferOptions == I2C_FIRST_FRAME)  || (CurrentXferOptions == SMBUS_NO_OPTION_FRAME))
  {
    /* Generate Start */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);
  }
  else if (hi2c->PreviousState == SMBUS_STATE_MASTER_BUSY_TX)
  {
    /* Generate ReStart */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);
  }
  else
  {
    /* Do nothing */
  }

  /* Wait until SB flag is set */
  if (SMBUS_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (hi2c->Init.AddressingMode == I2C_ADDRESSINGMODE_7BIT)
  {
    /* Send slave address */
    hi2c->Instance->DR = I2C_7BIT_ADD_READ(DevAddress);
  }
  else
  {
    /* Send header of slave address */
    hi2c->Instance->DR = I2C_10BIT_HEADER_WRITE(DevAddress);

    /* Wait until ADD10 flag is set */
    if (SMBUS_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADD10, Timeout, Tickstart) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Send slave address */
    hi2c->Instance->DR = I2C_10BIT_ADDRESS(DevAddress);

    /* Wait until ADDR flag is set */
    if (SMBUS_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout, Tickstart) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(hi2c);

    /* Generate Restart */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_START);

    /* Wait until SB flag is set */
    if (SMBUS_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Send header of slave address */
    hi2c->Instance->DR = I2C_10BIT_HEADER_READ(DevAddress);
  }

  /* Wait until ADDR flag is set */
  if (SMBUS_WaitOnMasterAddressFlagUntilTimeout(hi2c, I2C_FLAG_ADDR, Timeout, Tickstart) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for specific usage of TXE flag.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef SMBUS_WaitOnTXEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{
  while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE) == RESET)
  {
    /* Check if a NACK is detected */
    if (SMBUS_IsAcknowledgeFailed(hi2c) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {
        hi2c->PreviousState       = SMBUS_STATE_NONE;
        hi2c->State               = HAL_I2C_STATE_READY;
        hi2c->Mode                = HAL_I2C_MODE_NONE;
        hi2c->ErrorCode           |= HAL_I2C_ERROR_TIMEOUT;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_ERROR;
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for specific usage of BTF flag.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef SMBUS_WaitOnBTFFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{
  while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF) == RESET)
  {
    /* Check if a NACK is detected */
    if (SMBUS_IsAcknowledgeFailed(hi2c) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {
        hi2c->PreviousState       = SMBUS_STATE_NONE;
        hi2c->State               = HAL_I2C_STATE_READY;
        hi2c->Mode                = HAL_I2C_MODE_NONE;
        hi2c->ErrorCode           |= HAL_I2C_ERROR_TIMEOUT;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_ERROR;
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for specific usage of RXNE flag.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef SMBUS_WaitOnRXNEFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Timeout, uint32_t Tickstart)
{

  while (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE) == RESET)
  {
    /* Check if a STOPF is detected */
    if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF) == SET)
    {
      /* Clear STOP Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_STOPF);

      hi2c->PreviousState       = SMBUS_STATE_NONE;
      hi2c->State               = HAL_I2C_STATE_READY;
      hi2c->Mode                = HAL_I2C_MODE_NONE;
      hi2c->ErrorCode           |= HAL_I2C_ERROR_NONE;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
    {
      hi2c->PreviousState       = SMBUS_STATE_NONE;
      hi2c->State               = HAL_I2C_STATE_READY;
      hi2c->Mode                = HAL_I2C_MODE_NONE;
      hi2c->ErrorCode           |= HAL_I2C_ERROR_TIMEOUT;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_ERROR;
    }
  }
  return HAL_OK;
}

/**
  * @brief  This function handles I2C Communication Timeout for Master addressing phase.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for I2C module
  * @param  Flag specifies the I2C flag to check.
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef SMBUS_WaitOnMasterAddressFlagUntilTimeout(I2C_HandleTypeDef *hi2c, uint32_t Flag, uint32_t Timeout, uint32_t Tickstart)
{
  while (__HAL_I2C_GET_FLAG(hi2c, Flag) == RESET)
  {
    if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
    {
      /* Generate Stop */
      SET_BIT(hi2c->Instance->CR1, I2C_CR1_STOP);

      /* Clear AF Flag */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

      hi2c->PreviousState       = SMBUS_STATE_NONE;
      hi2c->State               = HAL_I2C_STATE_READY;
      hi2c->Mode                = HAL_I2C_MODE_NONE;
      hi2c->ErrorCode           |= HAL_I2C_ERROR_AF;

      /* Process Unlocked */
      __HAL_UNLOCK(hi2c);

      return HAL_ERROR;
    }

    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {
        hi2c->PreviousState       = SMBUS_STATE_NONE;
        hi2c->State               = HAL_I2C_STATE_READY;
        hi2c->Mode                = HAL_I2C_MODE_NONE;
        hi2c->ErrorCode           |= HAL_I2C_ERROR_TIMEOUT;

        /* Process Unlocked */
        __HAL_UNLOCK(hi2c);

        return HAL_ERROR;
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  This function handles Acknowledge failed detection during an I2C Communication.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
static HAL_StatusTypeDef SMBUS_IsAcknowledgeFailed(I2C_HandleTypeDef *hi2c)
{
  if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF) == SET)
  {
    /* Clear NACKF Flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

    hi2c->PreviousState       = SMBUS_STATE_NONE;
    hi2c->State               = HAL_I2C_STATE_READY;
    hi2c->Mode                = HAL_I2C_MODE_NONE;
    hi2c->ErrorCode           |= HAL_I2C_ERROR_AF;

    /* Process Unlocked */
    __HAL_UNLOCK(hi2c);

    return HAL_ERROR;
  }
  return HAL_OK;
}

