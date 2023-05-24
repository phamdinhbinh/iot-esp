-- phpMyAdmin SQL Dump
-- version 5.2.0
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: May 01, 2023 at 09:52 AM
-- Server version: 10.4.25-MariaDB
-- PHP Version: 7.4.30

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `vuon_thong_minh`
--

-- --------------------------------------------------------

--
-- Table structure for table `history_data`
--

CREATE TABLE `history_data` (
  `id` int(10) UNSIGNED NOT NULL,
  `temp` float DEFAULT 0,
  `humi` float DEFAULT 0,
  `soil_moisture` float DEFAULT 0,
  `light` float DEFAULT 0,
  `date_time` datetime DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `stage`
--

CREATE TABLE `stage` (
  `stage_id` varchar(255) NOT NULL,
  `startDate` varchar(255) NOT NULL,
  `endDate` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Dumping data for table `stage`
--

INSERT INTO `stage` (`stage_id`, `startDate`, `endDate`) VALUES
('27322d99-f511-43a3-aeb7-e30cc5bc4aff', '01/05/2023', '05/05/2023'),
('a8cdeee1-02f3-44a8-80d7-35dc3089a949', '06/05/2023', '12/05/2023');

-- --------------------------------------------------------

--
-- Table structure for table `stage_item`
--

CREATE TABLE `stage_item` (
  `stage_item_id` int(11) NOT NULL,
  `device` int(11) NOT NULL,
  `mode` int(11) NOT NULL,
  `startPoint` int(11) NOT NULL,
  `endPoint` int(11) NOT NULL,
  `state` int(11) NOT NULL,
  `timeStart` varchar(255) NOT NULL,
  `timeEnd` varchar(255) NOT NULL,
  `stage_id` varchar(255) NOT NULL,
  `date` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Dumping data for table `stage_item`
--

INSERT INTO `stage_item` (`stage_item_id`, `device`, `mode`, `startPoint`, `endPoint`, `state`, `timeStart`, `timeEnd`, `stage_id`, `date`) VALUES
(9, 1, 1, 20, 76, 1, '01:02:04', '03:02:04', 'a8cdeee1-02f3-44a8-80d7-35dc3089a949', '12/05/2023'),
(10, 1, 1, 20, 76, 1, '01:02:04', '03:02:04', 'a8cdeee1-02f3-44a8-80d7-35dc3089a949', '11/05/2023'),
(11, 1, 1, 20, 76, 1, '01:02:04', '03:02:04', 'a8cdeee1-02f3-44a8-80d7-35dc3089a949', '10/05/2023'),
(12, 1, 1, 20, 76, 1, '01:02:04', '03:02:04', 'a8cdeee1-02f3-44a8-80d7-35dc3089a949', '07/05/2023'),
(13, 1, 1, 20, 76, 1, '01:02:04', '03:02:04', 'a8cdeee1-02f3-44a8-80d7-35dc3089a949', '09/05/2023'),
(14, 1, 1, 20, 76, 1, '01:02:04', '03:02:04', 'a8cdeee1-02f3-44a8-80d7-35dc3089a949', '08/05/2023'),
(15, 1, 1, 20, 76, 1, '01:02:04', '03:02:04', 'a8cdeee1-02f3-44a8-80d7-35dc3089a949', '06/05/2023');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `history_data`
--
ALTER TABLE `history_data`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `stage`
--
ALTER TABLE `stage`
  ADD PRIMARY KEY (`stage_id`);

--
-- Indexes for table `stage_item`
--
ALTER TABLE `stage_item`
  ADD PRIMARY KEY (`stage_item_id`),
  ADD KEY `stage_id` (`stage_id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `history_data`
--
ALTER TABLE `history_data`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=243;

--
-- AUTO_INCREMENT for table `stage_item`
--
ALTER TABLE `stage_item`
  MODIFY `stage_item_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=24;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
