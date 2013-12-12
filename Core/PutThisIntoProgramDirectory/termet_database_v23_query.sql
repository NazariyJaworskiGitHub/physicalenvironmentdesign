-- Created: 11.11.2012, 18:18

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `termet_db`
--

-- --------------------------------------------------------

--
-- Table: `analysis`
--

CREATE TABLE IF NOT EXISTS `analysis` (
  `Analys_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Project_ID` int(11) DEFAULT NULL,
  `All_fields` varchar(45) DEFAULT NULL,
  `Analys_Note` text,
  `User_ID` int(11) DEFAULT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`Analys_ID`)
) ENGINE=MyISAM DEFAULT CHARSET=cp1251 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table: `bcdata`
--

CREATE TABLE IF NOT EXISTS `bcdata` (
  `BCdata_ID` int(11) NOT NULL AUTO_INCREMENT,
  `BCset_ID` int(11) NOT NULL,
  `BC_Side` int(11) NOT NULL,
  `Temperature` float DEFAULT NULL,
  `HPower_p` float DEFAULT NULL,
  `HFlux_q` float DEFAULT NULL,
  `Conv_t` float DEFAULT NULL,
  `Conv_h` float DEFAULT NULL,
  `BitMask` int(11) DEFAULT NULL,
  `Bound_Note` text,
  `User_ID` int(11) DEFAULT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`BCdata_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=631 ;

-- --------------------------------------------------------

--
-- Table: `bcset`
--

CREATE TABLE IF NOT EXISTS `bcset` (
  `BCset_ID` int(11) NOT NULL AUTO_INCREMENT,
  `BCset_Name` varchar(256) DEFAULT NULL,
  `BCset_Short` varchar(16) DEFAULT NULL,
  `Project_ID` int(11) DEFAULT NULL,
  `Object_ID` int(11) DEFAULT NULL,
  `BCset_Stat` int(11) DEFAULT NULL,
  `BCset_Note` text,
  `User` int(11) DEFAULT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`BCset_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=68 ;

-- --------------------------------------------------------

--
-- Table: `cntsamples`
--

CREATE TABLE IF NOT EXISTS `cntsamples` (
  `CNTSample_ID` int(11) NOT NULL AUTO_INCREMENT,
  `CNT_Name` varchar(256) NOT NULL,
  `CNT_Short` varchar(16) DEFAULT NULL,
  `CNT_Type` int(11) NOT NULL,
  `CNT_Producer` varchar(256) DEFAULT NULL,
  `CNTCover_ID` int(11) NOT NULL,
  `CNTCover_Thickness` float NOT NULL,
  `CNT_Length` float NOT NULL,
  `CNT_Radius` float NOT NULL,
  `CNT_Mass` float DEFAULT NULL,
  `CNT_Density` float DEFAULT NULL,
  `CNT_ThermConduct_R` float DEFAULT NULL,
  `CNT_ThermConduct_L` float DEFAULT NULL,
  `CNT_HeatCapacity` float DEFAULT NULL,
  `CNT_ElectrConduct_R` float DEFAULT NULL,
  `CNT_ElectrConduct_L` float DEFAULT NULL,
  `CNT_Note` text,
  `User_ID` int(11) NOT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`CNTSample_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 COMMENT='dictionary' AUTO_INCREMENT=2 ;

-- --------------------------------------------------------

--
-- Table: `composites`
--

CREATE TABLE IF NOT EXISTS `composites` (
  `Comp_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Comp_Name` varchar(256) DEFAULT NULL,
  `Comp_Short` varchar(16) DEFAULT NULL,
  `CompType_ID` int(11) DEFAULT NULL,
  `CompParent_ID` int(11) DEFAULT NULL,
  `Proj_ID` int(11) DEFAULT NULL,
  `Component_No` int(11) DEFAULT NULL,
  `MatrixMat_ID` int(11) DEFAULT NULL,
  `Comp_Note` text,
  `User_ID` int(11) DEFAULT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`Comp_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=66 ;

-- --------------------------------------------------------

--
-- Table: `compstructures`
--

CREATE TABLE IF NOT EXISTS `compstructures` (
  `idCompStructures` int(11) NOT NULL AUTO_INCREMENT,
  `Composite_ID` int(11) DEFAULT NULL,
  `Material_ID` int(11) NOT NULL DEFAULT '0',
  `Component_No` int(11) DEFAULT NULL,
  `Layer_Height` float DEFAULT NULL,
  `CNTSample_ID` int(11) NOT NULL DEFAULT '0',
  `Orientation` int(11) DEFAULT NULL,
  `Concentration` float DEFAULT NULL,
  `Dimension` float DEFAULT NULL,
  `DimDeviation` float DEFAULT NULL,
  `FiberGenStrategy` varchar(1500) DEFAULT NULL,
  `TransitionalLayerOn` tinyint(1) NOT NULL DEFAULT '0',
  `TransitionalLayerHeight` float NOT NULL DEFAULT '0',
  `TransitionalLayerInProcent` tinyint(1) NOT NULL DEFAULT '0',
  `CompStructure_Note` text,
  `User_ID` int(11) DEFAULT NULL,
  `Last_Update` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`idCompStructures`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=192 ;

-- --------------------------------------------------------

--
-- Table: `comptypes`
--

CREATE TABLE IF NOT EXISTS `comptypes` (
  `CompType_ID` int(11) NOT NULL AUTO_INCREMENT,
  `CompType_Name` varchar(45) DEFAULT NULL,
  `CompType_No` varchar(45) DEFAULT NULL,
  `CompType_Pic` blob,
  `CompType_Note` text,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`CompType_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 COMMENT='dictionary' AUTO_INCREMENT=3 ;

-- --------------------------------------------------------

--
-- Table: `databaseversion`
--

CREATE TABLE IF NOT EXISTS `databaseversion` (
  `version` int(11) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=cp1251 COMMENT='dictionary';

-- --------------------------------------------------------

--
-- Table: `divisions`
--

CREATE TABLE IF NOT EXISTS `divisions` (
  `Division_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Division_Name` varchar(256) DEFAULT NULL,
  `Division_Short` varchar(16) DEFAULT NULL,
  `Division_Note` tinytext,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`Division_ID`)
) ENGINE=MyISAM DEFAULT CHARSET=cp1251 COMMENT='dictionary' AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table: `feagrids`
--

CREATE TABLE IF NOT EXISTS `feagrids` (
  `Grid_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Grid_Name` varchar(45) DEFAULT NULL,
  `Grid_Short` varchar(16) DEFAULT NULL,
  `Composite_ID` int(11) NOT NULL,
  `Grid_Dim` int(11) NOT NULL,
  `FE_Type` int(11) NOT NULL,
  `Num_Elem` int(11) NOT NULL,
  `Num_Nod` int(11) NOT NULL,
  `BandWidth` int(11) NOT NULL,
  `NodesPerWidth` int(11) NOT NULL,
  `NodesPerHeight` int(11) NOT NULL,
  `NodesPerLength` int(11) NOT NULL,
  `Zones` longblob,
  `Cord` longblob NOT NULL,
  `NOP` longblob NOT NULL,
  `Bord` longblob,
  `Grid_Note` text,
  `User_ID` int(11) NOT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`Grid_ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=849 ;

-- --------------------------------------------------------

--
-- Table: `feamaterials`
--

CREATE TABLE IF NOT EXISTS `feamaterials` (
  `FEAMat_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Grid_ID` int(11) NOT NULL,
  `Comp_ID` int(11) NOT NULL,
  `ZnMn` blob NOT NULL,
  `FEAMat_Note` text,
  `User` int(11) NOT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`FEAMat_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table: `fibersamples`
--

CREATE TABLE IF NOT EXISTS `fibersamples` (
  `FiberSample_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Fiber_Name` varchar(256) NOT NULL,
  `Fiber_Short` varchar(16) DEFAULT NULL,
  `Fiber_Type` int(11) NOT NULL,
  `Fiber_Producer` varchar(256) DEFAULT NULL,
  `Fiber_Material` int(11) NOT NULL,
  `FiberCover_ID` int(11) NOT NULL,
  `FiberCover_Thickness` float NOT NULL,
  `Fiber_Length` float NOT NULL,
  `Fiber_LengthDeviation` float DEFAULT NULL,
  `Fiber_Radius` float NOT NULL,
  `Fiber_RadiusDeviation` float DEFAULT NULL,
  `Fiber_ThermConduct_R` float DEFAULT NULL,
  `Fiber_ThermConduct_L` float DEFAULT NULL,
  `Fiber_HeatCapacity` float DEFAULT NULL,
  `Fiber_ElectrConduct_R` float DEFAULT NULL,
  `Fiber_ElectrConduct_L` float DEFAULT NULL,
  `Fiber_Note` text NOT NULL,
  `User_ID` int(11) NOT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`FiberSample_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 COMMENT='dictionary' AUTO_INCREMENT=21 ;

-- --------------------------------------------------------

--
-- Table: `heatsources`
--

CREATE TABLE IF NOT EXISTS `heatsources` (
  `heatflow_id` int(11) NOT NULL AUTO_INCREMENT,
  `bcdata_id` int(11) DEFAULT '0',
  `TEE` varchar(256) DEFAULT NULL,
  `XC` float DEFAULT NULL,
  `YC` float DEFAULT NULL,
  `AX` float DEFAULT NULL,
  `AY` float DEFAULT NULL,
  `TAU` float DEFAULT NULL,
  `RI` float DEFAULT NULL,
  `TAUK` float DEFAULT NULL,
  `impulse_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `last_update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`heatflow_id`),
  KEY `heatflow_id` (`heatflow_id`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=262 ;

-- --------------------------------------------------------

--
-- Table: `impulse`
--

CREATE TABLE IF NOT EXISTS `impulse` (
  `impulse_id` int(11) NOT NULL AUTO_INCREMENT,
  `dl` float DEFAULT NULL,
  `pr` float DEFAULT NULL,
  `u1` float DEFAULT NULL,
  `u2` float DEFAULT NULL,
  `u3` float DEFAULT NULL,
  `u4` float DEFAULT NULL,
  `u5` float DEFAULT NULL,
  `u6` float DEFAULT NULL,
  `user_id` int(11) NOT NULL DEFAULT '0',
  `last_update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`impulse_id`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=262 ;

-- --------------------------------------------------------

--
-- Table: `logins`
--

CREATE TABLE IF NOT EXISTS `logins` (
  `Login_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Login_Name` varchar(45) NOT NULL DEFAULT '',
  `Login_Pass` varchar(45) NOT NULL DEFAULT '',
  `User_ID` int(11) NOT NULL,
  `Role_ID` int(11) NOT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`Login_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=11 ;

-- --------------------------------------------------------

--
-- Table: `materialclasses`
--

CREATE TABLE IF NOT EXISTS `materialclasses` (
  `MatClass_ID` int(11) NOT NULL AUTO_INCREMENT,
  `MatClass_Name` varchar(256) NOT NULL,
  `MatClass_Note` text,
  `MatType_ID` int(11) NOT NULL DEFAULT '1',
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`MatClass_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=30 ;

-- --------------------------------------------------------

--
-- Table: `materials`
--

CREATE TABLE IF NOT EXISTS `materials` (
  `Mat_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Mat_Name` varchar(256) NOT NULL,
  `Mat_Short` varchar(16) NOT NULL,
  `Mat_Color` int(11) NOT NULL DEFAULT '0',
  `Mat_Texture` varchar(300) DEFAULT '0',
  `MatClass_ID` int(11) DEFAULT NULL,
  `MatType_ID` int(11) DEFAULT NULL,
  `Mat_Note` text,
  `User_ID` int(11) NOT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `Elect_Conductivity` double DEFAULT NULL,
  `Permittivity` double DEFAULT NULL,
  `Permeability` double DEFAULT NULL,
  `Dielect_Strength` double DEFAULT NULL,
  `Seebeck_Coeff` double DEFAULT NULL,
  `Specific_Heat` double DEFAULT NULL,
  `Therm_Conductivity_A0` double DEFAULT NULL,
  `Therm_Conductivity_A1` double NOT NULL DEFAULT '0',
  `Therm_Conductivity_A2` double NOT NULL DEFAULT '0',
  `Isotropic` tinyint(1) NOT NULL DEFAULT '0',
  `Therm_Conductivity_ISO_A0` double NOT NULL DEFAULT '0',
  `Therm_Conductivity_ISO_A1` double NOT NULL DEFAULT '0',
  `Therm_Conductivity_ISO_A2` double NOT NULL DEFAULT '0',
  `Thermal_Diffusivity` double DEFAULT NULL,
  `Therm_Expansion` double DEFAULT NULL,
  `Melting_Point` double DEFAULT NULL,
  `Surface_Energy` double DEFAULT NULL,
  `Surface_Tension` double DEFAULT NULL,
  `Reactivity` double DEFAULT NULL,
  `Corrosion_Resistance` double DEFAULT NULL,
  `Compress_Strength` double DEFAULT NULL,
  `Ductility` double DEFAULT NULL,
  `Fatigue_Limit` double DEFAULT NULL,
  `Hardness` double DEFAULT NULL,
  `Poisson_Ratio` double DEFAULT NULL,
  `Softness` double DEFAULT NULL,
  `Specific_Modulus` double DEFAULT NULL,
  `Tensile_Strength` double DEFAULT NULL,
  `Yield_Strength` double DEFAULT NULL,
  `Young_Modulus` double DEFAULT NULL,
  `Flexural_Modulus` double DEFAULT NULL,
  `Bulk_Modulus` double DEFAULT NULL,
  `Shear_Modulus` double DEFAULT NULL,
  `Molar_Mass` double DEFAULT NULL,
  `Specific_Weight` double DEFAULT NULL,
  `Density` double DEFAULT NULL,
  `Relative_Density` double DEFAULT NULL,
  PRIMARY KEY (`Mat_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=51 ;

-- --------------------------------------------------------

--
-- Table: `materialtypes`
--

CREATE TABLE IF NOT EXISTS `materialtypes` (
  `Mattypes_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Mattypes_Name` varchar(256) NOT NULL,
  `Mattypes_Note` text,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`Mattypes_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=8 ;

-- --------------------------------------------------------

--
-- Table: `members`
--

CREATE TABLE IF NOT EXISTS `members` (
  `Member_ID` int(11) NOT NULL AUTO_INCREMENT,
  `User_ID` int(11) DEFAULT NULL,
  `Login_ID` int(11) DEFAULT NULL,
  `Role_ID` int(11) DEFAULT NULL,
  `Project_ID` int(11) DEFAULT NULL,
  `User` int(11) DEFAULT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`Member_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=104 ;

-- --------------------------------------------------------

--
-- Table: `nestdata`
--

CREATE TABLE IF NOT EXISTS `nestdata` (
  `nestdata_id` int(11) NOT NULL AUTO_INCREMENT,
  `nu1` int(11) DEFAULT NULL,
  `it` int(11) DEFAULT NULL,
  `ni` int(11) DEFAULT NULL,
  `EPS` float DEFAULT NULL,
  `EPS1` float DEFAULT NULL,
  `EPS2` float DEFAULT NULL,
  `IDX` int(11) DEFAULT NULL,
  `IDY` int(11) DEFAULT NULL,
  `KE` int(11) DEFAULT NULL,
  `KZ` int(11) DEFAULT NULL,
  `KXY1` int(11) DEFAULT NULL,
  `KKT` int(11) DEFAULT NULL,
  `TK` float DEFAULT '0',
  `user_id` int(11) NOT NULL,
  `bcdata_id` int(11) DEFAULT NULL,
  `last_update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `construction_id` int(11) DEFAULT NULL,
  `compstructure_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`nestdata_id`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=109 ;

-- --------------------------------------------------------

--
-- Table: `nestresults`
--

CREATE TABLE IF NOT EXISTS `nestresults` (
  `nestresults_id` int(11) NOT NULL AUTO_INCREMENT,
  `results` blob,
  `nestdata_id` int(11) NOT NULL,
  `project_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `last_update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`nestresults_id`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 PACK_KEYS=1 AUTO_INCREMENT=104 ;

-- --------------------------------------------------------

--
-- Table: `objects`
--

CREATE TABLE IF NOT EXISTS `objects` (
  `Object_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Object_Type` int(11) NOT NULL,
  `Object_Width` float DEFAULT NULL,
  `Object_Height` float DEFAULT NULL,
  `Object_Length` float DEFAULT NULL,
  `Object_Radius` float DEFAULT NULL,
  `Object_Note` text,
  `User_ID` int(11) NOT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`Object_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=42 ;

-- --------------------------------------------------------

--
-- Table: `projects`
--

CREATE TABLE IF NOT EXISTS `projects` (
  `Proj_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Proj_Name` varchar(256) DEFAULT NULL,
  `Proj_Short` varchar(16) DEFAULT NULL,
  `Proj_StartDate` datetime DEFAULT NULL,
  `Object_ID` int(11) DEFAULT NULL,
  `Proj_Status` varchar(8) DEFAULT NULL,
  `Proj_EndDate` datetime DEFAULT NULL,
  `Proj_Note` varchar(45) DEFAULT NULL,
  `Proj_Manager` int(11) DEFAULT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`Proj_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=42 ;

-- --------------------------------------------------------

--
-- Table: `roles`
--

CREATE TABLE IF NOT EXISTS `roles` (
  `Role_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Role_Name` varchar(45) NOT NULL DEFAULT '',
  `Role_Description` varchar(255) NOT NULL DEFAULT '',
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`Role_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=4 ;

-- --------------------------------------------------------

--
-- Table: `simulations`
--

CREATE TABLE IF NOT EXISTS `simulations` (
  `Sim_ID` int(11) NOT NULL AUTO_INCREMENT,
  `Sim_Name` varchar(45) DEFAULT NULL,
  `Sim_Short` varchar(16) DEFAULT NULL,
  `Project_ID` int(11) NOT NULL,
  `Object_ID` int(11) NOT NULL,
  `Comp_ID` int(11) NOT NULL,
  `Bound_ID` int(11) NOT NULL,
  `Sim_Type` int(11) DEFAULT NULL,
  `Grid_ID` int(11) NOT NULL,
  `FEAMat_ID` int(11) NOT NULL,
  `FENodesHeat` longblob,
  `FEElemHeat` longblob,
  `NESTdata_ID` int(11) NOT NULL,
  `NESTheat` blob,
  `Sim_Time` time DEFAULT NULL,
  `Sim_Cycles` int(11) DEFAULT NULL,
  `Sim_Error` float DEFAULT NULL,
  `Sim_Status` varchar(8) NOT NULL,
  `Sim_Note` text,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `User_ID` int(11) NOT NULL,
  PRIMARY KEY (`Sim_ID`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=846 ;

-- --------------------------------------------------------

--
-- Table: `symresults`
--

CREATE TABLE IF NOT EXISTS `symresults` (
  `SymRes_ID` int(11) NOT NULL AUTO_INCREMENT,
  `SymRes_Name` varchar(128) DEFAULT NULL,
  `SymRes_Short` varchar(16) DEFAULT NULL,
  `Sym_ID` int(11) NOT NULL,
  `SymRes_Type` int(11) NOT NULL,
  `SymRes_Data` blob NOT NULL,
  `SymRes_Note` text,
  `User_ID` int(11) NOT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`SymRes_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1251 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table: `synthesis`
--

CREATE TABLE IF NOT EXISTS `synthesis` (
  `Synthes_ID INT` int(11) NOT NULL AUTO_INCREMENT,
  `Project_ID` int(11) DEFAULT NULL,
  `All_fields` varchar(45) DEFAULT NULL,
  `Synthes_Note` text,
  `User_ID` int(11) DEFAULT NULL,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`Synthes_ID INT`)
) ENGINE=MyISAM DEFAULT CHARSET=cp1251 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table: `transitionallayers`
--

CREATE TABLE IF NOT EXISTS `transitionallayers` (
  `TransitionalLayerID` int(11) NOT NULL AUTO_INCREMENT,
  `ParentID` int(11) NOT NULL DEFAULT '0',
  `Concentration` float NOT NULL DEFAULT '0',
  `MaterialID` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`TransitionalLayerID`),
  KEY `ParentID` (`ParentID`),
  KEY `Material` (`MaterialID`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_bin AUTO_INCREMENT=47 ;

-- --------------------------------------------------------

--
-- Table: `users`
--

CREATE TABLE IF NOT EXISTS `users` (
  `User_ID` int(11) NOT NULL AUTO_INCREMENT,
  `User_Name` varchar(45) NOT NULL DEFAULT '',
  `User_Surname` varchar(45) NOT NULL DEFAULT '',
  `User_PID` varchar(15) DEFAULT NULL,
  `User_eMail` varchar(255) DEFAULT NULL,
  `Division` int(11) DEFAULT NULL,
  `User_Note` text,
  `Last_Update` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`User_ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=cp1251 AUTO_INCREMENT=9 ;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
