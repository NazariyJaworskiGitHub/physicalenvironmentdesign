/// \author Nazariy Jaworski
/// \author Marian Lobur

#ifndef DATABASEUPDATER_H
#define DATABASEUPDATER_H

#include <QObject>
#include <QSqlDatabase>

namespace DataBase
{
    class DataBaseUpdater : public QObject
    {
        Q_OBJECT

            /// Current version
        private: static const int currentVersion = 23;
            /// Common constructor
        public : DataBaseUpdater(QObject *parent);
            /// Does the upgrades of specific database,
            /// note that database should be already opened
        public : void makeUpgrade(QString databaseName);
            /// Catch this signal with some Ui or Logger
        public : Q_SIGNAL void writeString(const QString message) const;
            /// Opened specific database
        private: QSqlDatabase _myDb;
            /// Does the change of database version
        private: void _changeVersion(int oldVersion, int newVersion);
            /// I don't have initial databes, so starting update is 23:\n
            /// \li ( 1) This methods creates in database table databaseversion
            /// \li ( 2) This methods change field BCset_ID in feagrids to Composite_ID
            /// \li ( 3) Add fields therm_conductivity_iso_a0...a2 and isotropic
            /// to the table materials
            /// \li ( 4) Change the type of name in bcset and add short name for bcset
            /// \li ( 5) Copy names of bcset to the Short names
            /// \li ( 6) Add 4 more fields for feagrids
            /// \li ( 7) In feagrids make change Short name attributes
            /// \li ( 8) Add name and int name to the simulations table
            /// \li ( 9) Change type of blob fields in feagrids and simulations
            /// \li (10) RenameCompositeMaterialId -> MatrixMat_ID
            /// \li (11) Material_ID and CNTsample_ID -> not null default 0
            /// \li (12) Merge reinforce% and dropConcentration fields as Concentration
            /// \li (13) DropRadius -> Dimension
            /// \li (14) Merge Deviation and DropRadiusDeviation as DimDeviation
            /// \li (15) Delete tables construction and layers
            /// \li (16) Add two rows to comptypes table
            /// \li (17) Recover database. For droplet composite to the database was written
            /// not real id of material
            /// \li (18) Create table fibersamples
            /// \li (19) Recover in database project manager information (rewrite user id by login id)
            /// \li (20) Delete all recordds from members table, and then insert all possible values
            /// \li (21) Changes compstructures table and creates new table for transitional layer
            /// \li (22) DataBaseUpdater::Upgrade22 - adding field in compstructures for
            /// saving generating strategy
            /// \li (23) Add new type and groups of materials
        private: bool _upgrade23();
    };
}

#endif // DATABASEUPDATER_H
